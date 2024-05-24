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

#ifndef FEDERATION_HH
#define FEDERATION_HH

#include <config.h>
#include <vector>
#include <RTI.hh>
#include "Entity.hh"
#include "ContainerEntity.hh"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

using namespace std ;

#define NODE_OBJECT_MODEL (const xmlChar*) "objectModel"
#define NODE_OBJECTS (const xmlChar*) "objects"
#define NODE_OBJECT_CLASS (const xmlChar*) "objectClass"
#define NODE_INTERACTIONS (const xmlChar*) "interactions"
#define NODE_INTERACTION_CLASS (const xmlChar*) "interactionClass"
#define NODE_ATTRIBUTE (const xmlChar*) "attribute"
#define NODE_PARAMETER (const xmlChar*) "parameter"
#define NODE_ROUTING_SPACE (const xmlChar*) "routingSpace"
#define NODE_DIMENSIONS (const xmlChar*) "dimensions"
#define NODE_DIMENSION (const xmlChar*) "dimension"

#define ATTRIBUTE_NAME (const xmlChar*) "name"
#define ATTRIBUTE_TRANSPORTATION (const xmlChar*) "transportation"
#define ATTRIBUTE_ORDER (const xmlChar*) "order"
#define ATTRIBUTE_SPACE (const xmlChar*) "space"

#define VALUE_RELIABLE (const xmlChar*) "HLAreliable"
#define VALUE_BESTEFFORT (const xmlChar*) "HLAbestEffort"
#define VALUE_TSO (const xmlChar*) "TimeStamp"
#define VALUE_RO (const xmlChar*) "Receive"

typedef Entity<RTI::AttributeHandle> Attr ;
typedef Entity<RTI::ParameterHandle> Param ;
typedef ContainerEntity<RTI::ObjectClassHandle, Attr> ObjClass ;
typedef ContainerEntity<RTI::InteractionClassHandle, Param> IntClass ;

typedef Entity<RTI::ObjectHandle> Obj ;

class Federation 
{
public:
    Federation(RTI::RTIambassador*, string);
    ~Federation();  

    void setVerbose(bool);

    void update();
    void connect(Federation&);
    void setId(int);
    void resign(RTI::FedTime &);
  
    RTI::ObjectClassHandle getObjectClassHandle(string);
    RTI::AttributeHandle getAttributeHandle(string);
    RTI::InteractionClassHandle getInteractionClassHandle(string);
    RTI::ParameterHandle getParameterHandle(string);

    void publishAll(void);
    void subscribeAll(void);

    void discoverObject(RTI::ObjectHandle, string);
    void removeObject(RTI::ObjectHandle);

    void addObjectTranslation(RTI::ObjectHandle, RTI::ObjectHandle);
    RTI::ObjectHandle getObjectTranslation(int, RTI::ObjectHandle);
    RTI::ObjectClassHandle getObjectClassTranslation(int, RTI::ObjectClassHandle);
    RTI::InteractionClassHandle getInteractionClassTranslation(int, RTI::InteractionClassHandle);

    bool objectExists(RTI::ObjectHandle);

    bool empty(void);

private:
    void updateObjectClasses(vector<ObjClass>&);
    void updateInteractionClasses(vector<IntClass>&);
    void addAttribute(vector<ObjClass>&, string, string);
    void addSubObjectClass(string, string);
    void addSubInteractionClass(string, string);
    void addParameter(string, string);
    RTI::ObjectClassHandle searchObjectClassHandle(vector<ObjClass>&, string&);
    RTI::InteractionClassHandle searchInteractionClassHandle(vector<ObjClass>&, 
                                                        string&);
    RTI::AttributeHandle searchAttributeHandle(vector<ObjClass>&, string&);
    RTI::ParameterHandle searchParameterHandle(vector<IntClass>&, string&);

    RTI::ObjectClassHandle searchObjectClassTranslation(vector<IntClass>&, int, 
                                                   RTI::ObjectClassHandle);

    void connectObjectClasses(vector<ObjClass>&, Federation&);
    void connectInteractionClasses(vector<IntClass>&, Federation&);

    void publishAllObjectClasses(vector<ObjClass>&);
    void publishAllInteractionClasses(vector<IntClass>&);
    void subscribeAllObjectClasses(vector<ObjClass>&);
    void subscribeAllInteractionClasses(vector<IntClass>&);

    void dump(void);

    int parse(string);
    void parseClass(string);
    void parseInteraction(string);

    RTI::RTIambassador* rtiamb ;
    vector<ObjClass> sobj ;
    vector<IntClass> sint ;
    vector<Obj> dobj ;

    int translations ;
    int id ;
    bool verbose ;

    xmlDocPtr doc ;
    xmlNodePtr cur ;
};

#endif // FEDERATION_HH


