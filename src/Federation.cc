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

#include "Federation.hh"

// ---------------------------------------------------------------------------
// Federation
// 
Federation::Federation(RTI::RTIambassador* rti, string fedfile)
{
    rtiamb = rti ;
    translations = 0 ;
    verbose = false ;
    id = -1 ;

    this->parse(fedfile);
}

// ---------------------------------------------------------------------------
// ~Federation
// 
Federation::~Federation()
{
    sobj.clear();
    sint.clear();
}

// ---------------------------------------------------------------------------
// update
// 
void
Federation::update(void)
{
    updateObjectClasses(sobj);
    updateInteractionClasses(sint);
}

// ---------------------------------------------------------------------------
// updateObjectClasses
// 
void 
Federation::updateObjectClasses(vector<ObjClass> &obj)
{
    for(vector<ObjClass>::iterator i=obj.begin(); i!=obj.end(); i++) {
        const char* classname = i->getName().c_str();
        RTI::ObjectClassHandle h = rtiamb->getObjectClassHandle(classname) ;
        i->setHandle(h);
        vector<Attr> &a = i->getAttributes() ;
        for(vector<Attr>::iterator j=a.begin(); j!=a.end(); j++) {
            j->setHandle(rtiamb->getAttributeHandle(j->getName().c_str(), h));
        }
        vector<ObjClass> &sub = i->getSubEntities();
        if(sub.size()>0) this->updateObjectClasses(sub);
    }
}

// ---------------------------------------------------------------------------
// updateInteractionClasses
// 
void 
Federation::updateInteractionClasses(vector<IntClass> &intc)
{
    for(vector<IntClass>::iterator i=intc.begin(); i!=intc.end(); i++) {
        const char* classname = i->getName().c_str();
        RTI::InteractionClassHandle h = rtiamb->getInteractionClassHandle(classname);
        i->setHandle(h);    
        vector<Param> &p = i->getAttributes() ;
        for(vector<Param>::iterator j=p.begin(); j!=p.end(); j++) {
            j->setHandle(rtiamb->getParameterHandle(j->getName().c_str(), h));
        }    
        vector<IntClass> &sub = i->getSubEntities();
        if(sub.size()>0) this->updateInteractionClasses(sub);
    }
}

// ---------------------------------------------------------------------------
// addAttribute
// 
void
Federation::addAttribute(vector<ObjClass> &obj, string c, string a)
{
    for(vector<ObjClass>::iterator i=obj.begin(); i!=obj.end(); i++) {
        if(!i->getName().compare(c)) i->addAttribute(a) ;  
        vector<ObjClass> &sub = i->getSubEntities();
        if(sub.size()>0) this->addAttribute(sub, c, a);
    }
}

// ---------------------------------------------------------------------------
// addSubObjectClass
// 
void
Federation::addSubObjectClass(string p, string c)
{
    for(vector<ObjClass>::iterator i=sobj.begin(); i!=sobj.end(); i++) {
        if(!i->getName().compare(p)) i->addSubEntity(c) ;  
        // + les sous classes
    }
}

// ---------------------------------------------------------------------------
// addSubInteractionClass
// 
void
Federation::addSubInteractionClass(string p, string c)
{
    for(vector<ObjClass>::iterator i=sint.begin(); i!=sint.end(); i++) {
        if(!i->getName().compare(p)) i->addSubEntity(c) ;  
        // + les sous classes
    }
}


// ---------------------------------------------------------------------------
// addParameter
// 
void
Federation::addParameter(string c, string a)
{
    for(vector<IntClass>::iterator i=sint.begin(); i!=sint.end(); i++)
        if(!i->getName().compare(c)) i->addAttribute(a) ;  
}

// ---------------------------------------------------------------------------
// dump
// 
void
Federation::dump(void)
{
    if (verbose) {
        for (vector<ObjClass>::iterator i=sobj.begin(); i!=sobj.end(); i++) {
            i->dump();
        }
        for (vector<IntClass>::iterator i=sint.begin(); i!=sint.end(); i++) {
            i->dump();
        }
    }
}

// ---------------------------------------------------------------------------
// connect
// 
void 
Federation::connect(Federation &f)
{
    this->connectObjectClasses(sobj, f);
    this->connectInteractionClasses(sint, f);
    translations++ ;
}

// ---------------------------------------------------------------------------
// connectObjectClasses
// 
void
Federation::connectObjectClasses(vector<ObjClass> &v, Federation &f)
{
    for(vector<ObjClass>::iterator i=v.begin(); i!= v.end(); i++) {
        i->addTranslation(f.getObjectClassHandle(i->getName()));
        this->connectObjectClasses(i->getSubEntities(), f);
        vector<Attr> &attr = i->getAttributes();
        for(vector<Attr>::iterator j=attr.begin(); j!=attr.end(); j++) {
            j->addTranslation(f.getAttributeHandle(j->getName()));
        }
    }
}

// ---------------------------------------------------------------------------
// connectInteractionClasses
// 
void
Federation::connectInteractionClasses(vector<IntClass> &v, Federation &f)
{
    for(vector<IntClass>::iterator i=v.begin(); i!= v.end(); i++) {
        i->addTranslation(f.getInteractionClassHandle(i->getName()));
        vector<IntClass> &sub=i->getSubEntities();
        this->connectInteractionClasses(sub, f);
        vector<Param> &param = i->getAttributes();
        for(vector<Param>::iterator j=param.begin(); j!=param.end(); j++) {
            j->addTranslation(f.getParameterHandle(j->getName()));
        }
    }
}

// ---------------------------------------------------------------------------
// getObjectClassHandle
// 
RTI::ObjectClassHandle
Federation::getObjectClassHandle(string s)
{
    return this->searchObjectClassHandle(sobj, s) ;
}

// ---------------------------------------------------------------------------
// getAttributeHandle
// 
RTI::AttributeHandle
Federation::getAttributeHandle(string s)
{
    return this->searchAttributeHandle(sobj, s) ;
}

// ---------------------------------------------------------------------------
// getInteractionClassHandle
// 
RTI::InteractionClassHandle
Federation::getInteractionClassHandle(string s)
{
    return this->searchInteractionClassHandle(sint, s) ;
}

// ---------------------------------------------------------------------------
// getParameterHandle
// 
RTI::ParameterHandle
Federation::getParameterHandle(string s)
{
    return this->searchParameterHandle(sint, s);
}

// ---------------------------------------------------------------------------
// searchObjectClassHandle
// 
RTI::ObjectClassHandle
Federation::searchObjectClassHandle(vector<ObjClass> &v, string &s)
{
    for(vector<ObjClass>::iterator i=v.begin(); i!=v.end(); i++) {
        if(!i->getName().compare(s)) return i->getHandle();
        else return this->searchObjectClassHandle(i->getSubEntities(), s);
    }
    return 0 ;
}

// ---------------------------------------------------------------------------
// searchAttributeHandle
// 
RTI::AttributeHandle
Federation::searchAttributeHandle(vector<ObjClass> &v, string &s)
{
    for(vector<ObjClass>::iterator i=v.begin(); i!=v.end(); i++) {
        vector<Attr> &attr = i->getAttributes();
        for(vector<Attr>::iterator j=attr.begin(); j!=attr.end(); j++) {
            if(!j->getName().compare(s)) return j->getHandle();
        }
        RTI::AttributeHandle a = this->searchAttributeHandle(i->getSubEntities(), s);
        if(a) return a ;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// searchInteractionClassHandle
// 
RTI::InteractionClassHandle
Federation::searchInteractionClassHandle(vector<IntClass> &v, string &s)
{
    for(vector<IntClass>::iterator i=v.begin(); i!=v.end(); i++) {
        if(!i->getName().compare(s)) return i->getHandle();
        else return this->searchInteractionClassHandle(i->getSubEntities(), s);
    }
    return 0 ;
}

// ---------------------------------------------------------------------------
// searchParameterHandle
// 
RTI::ParameterHandle
Federation::searchParameterHandle(vector<IntClass> &v, string &s)
{
    for(vector<IntClass>::iterator i=v.begin(); i!=v.end(); i++) {
        vector<Param> &param = i->getAttributes();
        for(vector<Param>::iterator j=param.begin(); j!=param.end(); j++) {
            if(!j->getName().compare(s)) return j->getHandle();
        }
        RTI::ParameterHandle p = this->searchParameterHandle(i->getSubEntities(), s);
        if(p) return p ;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// publishAll
// 
void
Federation::publishAll(void)
{
    this->publishAllObjectClasses(sobj);
    this->publishAllInteractionClasses(sint); 
    if (verbose) {
        cout << "Federation(" << id << ") - Publications done" << endl ;
    }
}

// ---------------------------------------------------------------------------
// publishAllObjectClasses
// 
void
Federation::publishAllObjectClasses(vector<ObjClass> &v)
{
    for(vector<ObjClass>::iterator i=v.begin(); i!=v.end(); i++) {
        // Preparing attributes
        vector<Attr> &attr = i->getAttributes();
        RTI::AttributeHandleSet *handles ;
        handles = RTI::AttributeHandleSetFactory::create(i->getAttributes().size());    
        for(vector<Attr>::iterator a=attr.begin(); a!=attr.end(); a++) {
            handles->add(a->getHandle());
        }

        // publish
        rtiamb->publishObjectClass(i->getHandle(), *handles);
        handles->empty();

        // Now the sub classes
        this->publishAllObjectClasses(i->getSubEntities());
    }
}

// ---------------------------------------------------------------------------
// publishAllInteractionClasses
// 
void
Federation::publishAllInteractionClasses(vector<IntClass> &v)
{
    for(vector<IntClass>::iterator i=v.begin(); i!=v.end(); i++) {    
        rtiamb->publishInteractionClass(i->getHandle());
        //this->publishAllInteractionClasses(i->getSubEntities()); 
    }
}

// ---------------------------------------------------------------------------
// subscribeAll
// 
void
Federation::subscribeAll(void)
{
    this->subscribeAllObjectClasses(sobj);
    this->subscribeAllInteractionClasses(sint); 
    if (verbose) {
        cout << "Federation(" << id << ") - Subscriptions done" << endl ;
    }
}
// ---------------------------------------------------------------------------
// subscribeAllObjectClasses
// 
void
Federation::subscribeAllObjectClasses(vector<ObjClass> &v)
{
    for(vector<ObjClass>::iterator i=v.begin(); i!=v.end(); i++) {
        // Preparing attributes
        vector<Attr> &attr = i->getAttributes();
        RTI::AttributeHandleSet *handles ;
        handles = RTI::AttributeHandleSetFactory::create(i->getAttributes().size());    
        for(vector<Attr>::iterator a=attr.begin(); a!=attr.end(); a++) {
            handles->add(a->getHandle());
        }

        // publish
        rtiamb->subscribeObjectClassAttributes(i->getHandle(), *handles);
        handles->empty();

        // Now the sub classes
        // Commented out: CERTI would send 2 RAV for the same UAV
        // (eg. both as Bille object and as Boule object)
        //this->subscribeAllObjectClasses(i->getSubEntities());        
    }
}

// ---------------------------------------------------------------------------
// subscribeAllInteractionClasses
// 
void
Federation::subscribeAllInteractionClasses(vector<IntClass> &v)
{
    for(vector<IntClass>::iterator i=v.begin(); i!=v.end(); i++) {    
        rtiamb->subscribeInteractionClass(i->getHandle());
        //this->subscribeAllInteractionClasses(i->getSubEntities()); 
    }
}

// ---------------------------------------------------------------------------
// addTranslation
// 
void
Federation::addObjectTranslation(RTI::ObjectHandle object, RTI::ObjectHandle surrogate)
{
    for(vector<Obj>::iterator i=dobj.begin(); i!=dobj.end(); i++) {
        if(i->getHandle()==object) i->addTranslation(surrogate);
    }
}

// ---------------------------------------------------------------------------
// discoverObject
// 
void
Federation::discoverObject(RTI::ObjectHandle handle, string name)
{
    if(!this->objectExists(handle)) {
        dobj.push_back(Obj(name, handle));
    }
    else cout << "WARNING: Federation RE-discovers object " << handle
              << endl ;
}

// ---------------------------------------------------------------------------
// removeObject
// 
void 
Federation::removeObject(RTI::ObjectHandle handle)
{
    if(this->objectExists(handle)) {
        //    printf("Federation removes object %u\n", handle);
        for(vector<Obj>::iterator i=dobj.begin(); i!=dobj.end(); i++) {
            if(i->getHandle()==handle) {
                dobj.erase(i);
                return ;
            }
        }
    }
    else printf("WARNING: Federation asked to remove unknown object %u", handle);
}

// ---------------------------------------------------------------------------
// objectExists
//
bool
Federation::objectExists(RTI::ObjectHandle handle)
{
    for(vector<Obj>::iterator i=dobj.begin(); i!=dobj.end(); i++) {
        if(i->getHandle()==handle) return true ;
    }
    return false ;
}

// ---------------------------------------------------------------------------
// getObjectTranslation
// 
RTI::ObjectHandle 
Federation::getObjectTranslation(int n, RTI::ObjectHandle object)
{
    for(vector<Obj>::iterator i=dobj.begin(); i!=dobj.end(); i++) {
        if(i->getHandle()==object) return i->getTranslation(n);     
    }
}

// ---------------------------------------------------------------------------
// getObjectClassTranslation
// 
RTI::ObjectClassHandle 
Federation::getObjectClassTranslation(int n, RTI::ObjectClassHandle object)
{
    return this->searchObjectClassTranslation(sobj, n, object);
}

// ---------------------------------------------------------------------------
// searchObjectClassTranslation
//
RTI::ObjectClassHandle
Federation::searchObjectClassTranslation(vector<ObjClass> &v, int n, 
                                         RTI::ObjectClassHandle object)
{
    for(vector<ObjClass>::iterator i=v.begin(); i!=v.end(); i++) {
        if(i->getHandle()==object) return i->getTranslation(n);
        else return this->searchObjectClassTranslation(i->getSubEntities(), 
                                                       n, object);
    }
    return 0 ;
}

// ---------------------------------------------------------------------------
// getInteractionClassTranslation
// 
RTI::InteractionClassHandle
Federation::getInteractionClassTranslation(int n, 
                                           RTI::InteractionClassHandle interaction)
{
    for(vector<IntClass>::iterator i=sint.begin(); i!=sint.end(); i++) {
        if(i->getHandle()==interaction) return i->getTranslation(n);     
    }
}

// ---------------------------------------------------------------------------
// setId
//
void
Federation::setId(int n)
{
    id = n ;
}

// ---------------------------------------------------------------------------
// empty
//
bool
Federation::empty(void)
{
    return dobj.size() > 0 ;
}

// ---------------------------------------------------------------------------
// resign : delete dobj objects
//
void
Federation::resign(RTI::FedTime &t)
{
    for(vector<Obj>::iterator i=dobj.begin(); i!=dobj.end(); i++) {
        rtiamb->deleteObjectInstance(i->getHandle(), t, "");
    }    
}

// ---------------------------------------------------------------------------
// setVerbose
//
void
Federation::setVerbose(bool v)
{
    verbose = v ;
}

// ----------------------------------------------------------------------------
//! Main method to parse .xml file
int
Federation::parse(string filename)
{
    doc = xmlParseFile(filename.c_str());

    // Did libXML manage to parse the file ?
    if (doc == 0) {
        cerr << "XML file not parsed successfully" << endl ;
        xmlFreeDoc(doc);
        return 1 ;
    }

    // Is there a root element ?
    cur = xmlDocGetRootElement(doc);
    if (cur == 0) {
        cerr << "XML file is empty" << endl ;
        xmlFreeDoc(doc);
        return 1 ;
    }

    // Is this root element an objectModel ?
    if (xmlStrcmp(cur->name, (const xmlChar *) NODE_OBJECT_MODEL)) {
        cerr << "Wrong XML file: not the expected root node" << endl ;
        return 1 ;
    }
    if (verbose)
        cout << "XML file looks ok, starting main loop" << endl ;

    // Loop
    cur = xmlDocGetRootElement(doc);
    cur = cur->xmlChildrenNode ;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, NODE_OBJECTS))) {
            xmlNodePtr prev = cur ;
            cur = cur->xmlChildrenNode ;
            while (cur != NULL) {
                if ((!xmlStrcmp(cur->name, NODE_OBJECT_CLASS))) {
                    this->parseClass("");
                }
                cur = cur->next ;
            }
            cur = prev ;
        }
        if ((!xmlStrcmp(cur->name, NODE_INTERACTIONS))) {
            xmlNodePtr prev = cur ;
            cur = cur->xmlChildrenNode ;
            while (cur != NULL) {
                if ((!xmlStrcmp(cur->name, NODE_INTERACTION_CLASS))) {
                    this->parseInteraction("");
                }
                cur = cur->next ;
            }
            cur = prev ;
        }
        cur = cur->next ;
    }
    xmlFreeDoc(doc);
}

// ----------------------------------------------------------------------------
//! Parse the current class node
void
Federation::parseClass(string parent)
{
    xmlNodePtr prev = cur ;
    const char *current = (char *) xmlGetProp(cur, ATTRIBUTE_NAME);

    if (parent == "")
        sobj.push_back(ObjClass(current));
    else
        this->addSubObjectClass(parent, current);

    cur = cur->xmlChildrenNode ;
    while (cur != NULL) {
        // Attributes
        if ((!xmlStrcmp(cur->name, NODE_ATTRIBUTE))) {
            this->addAttribute(sobj, current,
                               (char *) xmlGetProp(cur, ATTRIBUTE_NAME));
        }
        // Subclasses
        if ((!xmlStrcmp(cur->name, NODE_OBJECT_CLASS))) {
            this->parseClass(current);
        }
        cur = cur->next ;
    }
    cur = prev ;
}

// ----------------------------------------------------------------------------
//! Parse the current interaction node
void
Federation::parseInteraction(string parent)
{
    xmlNodePtr prev = cur ;
    const char *current = (char *) xmlGetProp(cur, ATTRIBUTE_NAME);

    if (parent == "")
        sint.push_back(IntClass(current));
    else
        this->addSubInteractionClass(parent, current);

    cur = cur->xmlChildrenNode ;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, NODE_PARAMETER))) {
            this->addParameter(current,
                               (char *) xmlGetProp(cur, ATTRIBUTE_NAME));
        }
        // Subinteraction
        if ((!xmlStrcmp(cur->name, NODE_INTERACTION_CLASS))) {
            this->parseInteraction(current);
        }
        cur = cur->next ;
    }
    cur = prev ;
}
