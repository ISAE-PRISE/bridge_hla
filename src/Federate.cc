//----------------------------------------------------------------------
//----------------------------------------------------------------------
//
// bridge_hla 
// Implementation for IEEE HLA bridges between federations (Compliant with CERTI RTI)
// 
// Copyright (C) 2023  ISAE-SUPAERO
//
// Authors: Benoit Breholée
//          Jean-Baptiste Chaudron
//
// email:   jean-baptiste.chaudron@isae-supaero.fr
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------

#include "Federate.hh"
#include <stdio.h> // debug
#include <unistd.h>

// ----------------------------------------------------------------------------
// Federate
//
Federate::Federate(string federation_,
                   string federate_,
                   string fedfile_,
                   string host_,
                   string filter_)
    : localTime(0.0), localLBTS(0.0), globalLBTS(0.0), lookahead(0.5),
      timeRequest(0.5), minLookahead(0.1)
{
    federation = federation_ ;
    federate = federate_ ;
    fedfile = fedfile_ ;
    host = host_ ;
    filter = filter_ ;

    joined = false ;
    constrained = false ;
    regulating = false ;
    verbose = false ;
    paused = false ;
    id = -1 ;

    certihost = "CERTI_HOST=" + host ;
    putenv((char *) certihost.c_str());

    rtiamb = new RTI::RTIambassador();
    f = new Federation(rtiamb, fedfile);
    fedamb = new Fed(rtiamb, this, f);
}

// ----------------------------------------------------------------------------
// ~Federate
//
Federate::~Federate()
{
    if (joined) this->resign();
    delete f ;
    delete fedamb ;
    delete rtiamb ;
}

// ----------------------------------------------------------------------------
// setId
//
void
Federate::setId(int i)
{
    id = i ;
    fedamb->setId(id);
    f->setId(id);    
}

// ----------------------------------------------------------------------------
// setSynchro
//
void
Federate::setSynchro(string s)
{
    synchro = s ;
}

// ----------------------------------------------------------------------------
// join
//
int
Federate::join(void)
{
    int tries = 0 ;

    if (joined) return 0 ;

    while (!joined && tries < 3) {
        tries++ ;
        if (verbose) {
            cout << "Federate(" << id << ") - Trying to join federation "
                 << federation << "..." << endl ;
        }
        try {
            handle = rtiamb->joinFederationExecution(federate.c_str(),
                                                     federation.c_str(),
                                                     fedamb);
            joined=true ;
        }
        catch (RTI::Exception &e) {
            sleep(1);
        }
    }

    if (joined) {
        if (verbose) {
            cout << "Federate(" << id << ") - Joined federation "
                 << federation << endl ;
        }
    }
    else return -1 ;

    f->update();
    return 0 ;
}

// ----------------------------------------------------------------------------
// resign
//
int
Federate::resign(void)
{
    //    f->resign(RTIfedTime(localTime->getTime() + lookahead->getTime()));

    try {
        rtiamb->
            resignFederationExecution(RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
        if (verbose) {
            cout << "Federate(" << id << ") - Resigned federation " 
                 << federation << endl ;
        }
        return 0 ;
    }
    catch (RTI::Exception &e) {
    }
    return -1 ;
}

// ----------------------------------------------------------------------------
// connect
//
void
Federate::connect(Federate &other)
{
    feds.push_back(&other);
    f->connect(other.getFederation());
}

// ----------------------------------------------------------------------------
// init
//
void
Federate::init(void)
{
    f->publishAll();
    f->subscribeAll();
    this->setConstrained(true);
    this->setRegulating(true);
    this->synchronize();
}

// ----------------------------------------------------------------------------
// getFederation
//
Federation &
Federate::getFederation(void)
{
    return *f ;
}

// ----------------------------------------------------------------------------
// info
//
void
Federate::info(void)
{
    cout << "--- Federate info ---" << endl ;
    cout << "> federate : " << federate << endl ;
    cout << "> federation : " << federation << endl ;
    cout << "> fed file : " << fedfile << endl ;
    printf("> RTI ambassador -> %p\n", rtiamb);
    printf("> Federate ambassador -> %p\n", fedamb);
    cout << "</info>" << endl ;
}

// ----------------------------------------------------------------------------
// getObjectClassHandle
//
RTI::ObjectClassHandle
Federate::getObjectClassHandle(string s)
{
    if (f) return f->getObjectClassHandle(s);
}


// ----------------------------------------------------------------------------
// setConstrained
//
void
Federate::setConstrained(bool constrained_) {

    if (constrained_) {
        rtiamb->enableTimeConstrained();
        constrained = true ;
        if (verbose) {
            cout << "Federate(" << id << ") - Time Constrained Enabled" 
                 << endl ;
        }
    }
    else {
        rtiamb->disableTimeConstrained();
        constrained = false ;
        if (verbose) {
            cout << "Federate(" << id << ") - Time Constrained Disabled" 
                 << endl ;
        }
    }
}

// ----------------------------------------------------------------------------
// setRegulating
//
void
Federate::setRegulating(bool regulating_) {

    if (regulating_) {

        rtiamb->queryLBTS(localLBTS);

        while (true) {
            rtiamb->queryFederateTime(localTime);
            try {
                rtiamb->enableTimeRegulation(localTime, lookahead);
                break ;
            }
            catch (RTI::FederationTimeAlreadyPassed) {

                // Si Je ne suis pas le premier, je vais les rattraper.
                rtiamb->queryFederateTime(localTime);
                fedamb->setTAG(false);

                RTIfedTime requestTime(((RTIfedTime&) localTime).getTime());
                requestTime += localLBTS ;
                rtiamb->nextEventRequest(requestTime);

                while (!fedamb->getTAG()) {
                    try {
                        rtiamb->tick();
                    }
                    catch (RTI::RTIinternalError) {
                        printf ("RTIinternalError Raised in tick.\n");
                        exit (-1);
                    }
                }
            }
            catch (RTI::RTIinternalError) {
                printf ("RTIinternalError Raised in setTimeRegulating.\n");
                exit (-1);
            }
        }
        if (verbose) {
            cout << "Federate(" << id << ") - Time Regulation Enabled" << endl ;
        }
    }
    else {
        rtiamb->disableTimeRegulation();
        if (verbose) {
            cout << "Federate(" << id << ") - Time Regulation Disabled" 
                 << endl ;
        }
    }
    rtiamb->queryFederateTime(localTime);
}

// ----------------------------------------------------------------------------
// step
//
bool
Federate::step(void)
{
    //    rtiamb->tick();

    this->queryLBTS(localLBTS);
    this->updateGlobalLBTS();

    if (lookahead != minLookahead) {
        lookahead = minLookahead ;
        rtiamb->modifyLookahead(lookahead);
    }
    rtiamb->queryLookahead(lookahead);

    timeRequest = globalLBTS - lookahead ;

    if (verbose) {
        cout << endl << "Federate(" << id << ") -" 
             << " Time=" << localTime.getTime() 
             << " LBTS=" << localLBTS.getTime()
             << " GLBTS=" << globalLBTS.getTime()
             << " Lookahead=" << lookahead.getTime()
             << " Request=" << timeRequest.getTime() << endl ;
    }
    //        getchar();

    fedamb->setTAG(false);

    if (timeRequest > localTime) {
        try {   
            rtiamb->timeAdvanceRequest(timeRequest);
            // rtiamb->nextEventRequest(*time_aux);
        }
        catch (RTI::Exception& e) {
            cout << "[RTI::Exception: Advance Request]" ;
        }
        while (!fedamb->getTAG()) {
            try {
                rtiamb->tick();
            }
            catch (RTI::Exception& e) {
            }
        }
    }
    else {
        if (verbose) {
            cout << " | Request time is current time ; will not TAR" << endl ;
        }
    }

    try {
        rtiamb->queryFederateTime(localTime);
    }
    catch (RTI::Exception& e) {
        cout << "[RTI::Exception: Query Federate Time]" ;
    }

    return true ;
}

// ----------------------------------------------------------------------------
// updateLBTS
//
void
Federate::queryLBTS(RTIfedTime &t)
{
    rtiamb->queryLBTS(t);
}

// ----------------------------------------------------------------------------
// updateGlobalLBTS
//
void
Federate::updateGlobalLBTS(void)
{
    if (feds.size() == 0) 
        return ;

    RTIfedTime fedLBTS(0.0);
    bool first = true ;

    for (vector<Federate*>::iterator i = feds.begin() ; i != feds.end() ; i++) {
        (*i)->queryLBTS(fedLBTS);
        if (first) {
            globalLBTS = fedLBTS ;
            first = false ;            
        }
        else if (fedLBTS < globalLBTS) {
            globalLBTS = fedLBTS ;
        }
    }
}

// ----------------------------------------------------------------------------
// synchronize
//
void
Federate::synchronize(void)
{
    while (!paused) {
        try {
            rtiamb->tick();
        }
        catch (RTI::Exception& e) {
        }
    }

    try {
        rtiamb->synchronizationPointAchieved(synchro.c_str());
    }
    catch (RTI::Exception& e) {

    }

    cout << "Federation(" << id << ") - Synchronization..." << endl ;
    while (paused) {
        try {
            rtiamb->tick();
        }
        catch (RTI::Exception& e) {

        }
    }    
}

// ----------------------------------------------------------------------------
// announce
//
void
Federate::announce(string s)
{
    if (synchro == s)
        paused = true ;
    else
        if (verbose)
            cout << "Unknown synchronization point" << endl ;
}


// ----------------------------------------------------------------------------
// endSynchronization
//
void
Federate::endSynchronization(string s)
{
    if (synchro == s)
        paused = false ;
    else
        if (verbose)
            cout << "Unknown synchronization point" << endl ;
}

// ----------------------------------------------------------------------------
// discoverobject
//
void
Federate::discoverObject(RTI::ObjectHandle h,
                         RTI::ObjectClassHandle class_handle,
                         string name)
{
    bool discover ;

    int t=0 ;
    if (!f->objectExists(h)) {
        if (verbose) {
            cout << "Federate(" << id << ") - Discovers new object, handle "
                 << h << ", class " << class_handle << ", name " << name 
                 << endl ;
        }

        if (filter.empty() || filter.compare(0, filter.size() - 1, name)) {
            f->discoverObject(h, name);
            for (vector<Federate*>::iterator i=feds.begin(); 
                 i!=feds.end(); 
                 i++) {
                RTI::ObjectHandle surrogate = 
                    (*i)->
                    registerObject(f->getObjectClassTranslation(t, 
                                                                class_handle),
                                   name);
                f->addObjectTranslation(h, surrogate);
                t++ ;
            }
        } else {
            if (verbose) {
                cout << " | Note: this is an hidden object" << endl ;
            }
        }        
    }
    else {
        if (verbose) {
            cout << "Federate(" << id
                 << ") - Discovers existing object, handle " << handle
                 << ", class " << class_handle << ", name " << name 
                 << endl ;
        }
    }
}

// ----------------------------------------------------------------------------
// registerObject
//
RTI::ObjectHandle
Federate::registerObject(RTI::ObjectClassHandle class_handle, string name)
{
    if (verbose) {
        cout << "Federate(" << id << ") - Registers object named " << name 
             << ", class " << class_handle ;
    }

    RTI::ObjectHandle h = rtiamb->registerObjectInstance(class_handle, name.c_str());

    if (verbose) {
        cout << ", (proxy) handle " << h << endl ;
    }

    return h ;
}

// ----------------------------------------------------------------------------
// reflect
//
void
Federate::reflect(RTI::ObjectHandle object,
                  const RTI::AttributeHandleValuePairSet& attributes,
                  const RTI::FedTime& time)
{
    int t=0 ;
    for (vector<Federate*>::iterator i=feds.begin(); i!=feds.end(); i++) {
        RTI::ObjectHandle surrogate = f->getObjectTranslation(t, object);
        if (verbose) {
            cout << "Federate(" << id << ") - Reflects object " << surrogate ;
            cout << " (proxy of " << object << " in " << t << ") at " 
                 << ((RTIfedTime) time).getTime() << endl ;
        }

        (*i)->update(surrogate, attributes, time);
        t++ ;
    }
}

// ----------------------------------------------------------------------------
// receive
//
void
Federate::receive(RTI::InteractionClassHandle interaction,
                  const RTI::ParameterHandleValuePairSet& parameters,
                  const RTI::FedTime& time)
{
    int t=0 ;
    for (vector<Federate*>::iterator i=feds.begin(); i!=feds.end(); i++) {
        RTI::InteractionClassHandle surrogate =
            f->getInteractionClassTranslation(t, interaction);
        (*i)->send(surrogate, parameters, time);
        t++ ;
    }
}

// ----------------------------------------------------------------------------
// update
//
void
Federate::update(RTI::ObjectHandle object,
                 const RTI::AttributeHandleValuePairSet& attributes,
                 const RTI::FedTime& time)
{
    if (verbose) {
        cout << "Federate(" << id << ") - Update object " << object << " at "
             << ((RTIfedTime) time).getTime() << "..." ;
    }

    try {
        rtiamb->updateAttributeValues(object, attributes, time, "");
    }
    catch (RTI::Exception &e) {
        cout << "EXCEPTION " << e._reason << endl ;
        return ;
    }
    if (verbose) {
        cout << " done." << endl ;
    }
}

// ----------------------------------------------------------------------------
// send
void
Federate::send(RTI::InteractionClassHandle interaction,
               const RTI::ParameterHandleValuePairSet& parameters,
               const RTI::FedTime& time)
{
    if (verbose) {
        cout << "Federate(" << id << ") - Federate send proxy interaction " 
             << interaction << endl ;
    }

    rtiamb->sendInteraction(interaction, parameters, time, "");
}

// ----------------------------------------------------------------------------
// removeObject
//
void
Federate::removeObject(RTI::ObjectHandle object, const RTI::FedTime& time)
{
    int t=0 ;
    for (vector<Federate*>::iterator i=feds.begin(); i!=feds.end(); i++) {
        RTI::ObjectHandle surrogate = f->getObjectTranslation(t, object);
        (*i)->deleteObject(surrogate, time);
        f->removeObject(object);
        t++ ;
    }
}

// ----------------------------------------------------------------------------
// deleteObject
//
void
Federate::deleteObject(RTI::ObjectHandle object, const RTI::FedTime& time)
{
    if (verbose) {
        cout << "Federate(" << id << ") - Delete object " << object << endl ;
        rtiamb->deleteObjectInstance(object, time, "");
    }
}

// ---------------------------------------------------------------------------
// setVerbose
//
void
Federate::setVerbose(bool v)
{
    verbose = v ;
    f->setVerbose(verbose);
    fedamb->setVerbose(verbose);
}
