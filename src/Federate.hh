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

#ifndef FEDERATE_HH
#define FEDERATE_HH

#include <config.h>
#include <iostream>
#include <RTI.hh>
#include <fedtime.hh>
#include "Fed.hh"
#include <stdio.h>

#include "Federation.hh"

using std::cout ;
using std::endl ;

#define FEDERATE_JOIN_TRY_MAX 5
#define FEDERATE_ALREADY_JOINED -1
#define FEDERATE_UNABLE_TO_JOIN -2

using namespace std ;

class Fed ;

class Federate
{
    // ========================================================================
public:
    Federate(string, string, string, string, string);
    ~Federate();

    void setSynchro(string);

    RTI::RTIambassador* getRTIAmbassador(void);

    void connect(Federate&);
    Federation& getFederation(void);

    int join(void);
    void init(void);
    int resign(void);
    void synchronize(void);
    void info(void);
  
    RTI::ObjectClassHandle getObjectClassHandle(string);
    RTI::AttributeHandle getAttributeHandle(string);
    RTI::InteractionClassHandle getInteractionClassHandle(string);
    RTI::ParameterHandle getParameterHandle(string);

    bool step(void);

    void announce(string);
    void endSynchronization(string);

    void discoverObject(RTI::ObjectHandle, RTI::ObjectClassHandle, string);
    RTI::ObjectHandle registerObject(RTI::ObjectClassHandle, string);
    void reflect(RTI::ObjectHandle, 
                 const RTI::AttributeHandleValuePairSet&, 
                 const RTI::FedTime&);
    void update(RTI::ObjectHandle, 
                const RTI::AttributeHandleValuePairSet&,
                const RTI::FedTime&);
    void receive(RTI::InteractionClassHandle,
                 const RTI::ParameterHandleValuePairSet&,
                 const RTI::FedTime&);
    void send(RTI::InteractionClassHandle,
              const RTI::ParameterHandleValuePairSet&,
              const RTI::FedTime&);
    void removeObject(RTI::ObjectHandle, const RTI::FedTime&);
    void deleteObject(RTI::ObjectHandle, const RTI::FedTime&);

    void queryLBTS(RTIfedTime &);
    void updateGlobalLBTS(void);

    void setVerbose(bool);
    void setId(int);

    // ========================================================================
private:
    void publishAll(void);
    void subscribeAll(void);
    void setConstrained(bool);
    void setRegulating(bool);  

    RTI::RTIambassador* rtiamb ;
    Fed* fedamb ;
    vector<Federate*> feds ;
    Federation* f ;

    int id ;
    bool joined ;
    bool constrained ;
    bool regulating ;
    bool paused ;
    bool verbose ;
    RTI::FederateHandle handle ;

    RTIfedTime localTime ; // temps du federe
    RTIfedTime localLBTS ; // LBTS du federe
    RTIfedTime globalLBTS ; // min(LBTS) des autres federe
    RTIfedTime lookahead ; // lookahead du federe
    RTIfedTime timeRequest ; // timeStep avancement
    RTIfedTime minLookahead ; // lookahead min

    string federation ;
    string federate ;
    string fedfile ;
    string synchro ;
    string host ;
    string filter ;
    string certihost ;
};

#endif // FEDERATE_HH
