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

#ifndef FED_HH
#define FED_HH

#include <config.h>
#include "Federation.hh"
#include "Federate.hh"
#include <RTI.hh>
#include <NullFederateAmbassador.hh>
#include <fedtime.hh>

class Federate ;

class Fed : public NullFederateAmbassador
{ 
public:
    char CurrentPauseLabel[100] ;

private:
    RTI::RTIambassador* rtiamb ;
    bool granted ;
    int id ;
    bool verbose ;

    Federate* federate ;
    Federation* federation ;

public:
    Fed(RTI::RTIambassador*, Federate*, Federation*);
    virtual ~Fed();

    void setVerbose(bool);

    // ===== FederateAmbassador =============================================
public:
    void announceSynchronizationPoint(const char *label, const char *tag) 
        throw(RTI::FederateInternalError);

    void synchronizationPointRegistrationSucceeded(const char *label) 
        throw(RTI::FederateInternalError);

    void synchronizationPointRegistrationFailed(const char *label) 
        throw(RTI::FederateInternalError);

    void federationSynchronized(const char *label)
        throw(RTI::FederateInternalError);

    void removeObjectInstance(RTI::ObjectHandle theObject, 
                              const RTI::FedTime& theTime, 
                              const char* theTag, 
                              RTI::EventRetractionHandle theHandle) 
        throw(RTI::ObjectNotKnown, RTI::InvalidFederationTime, RTI::FederateInternalError) ;

    void timeAdvanceGrant(const RTI::FedTime& theTime) 
		throw ( RTI::InvalidFederationTime,
			RTI::TimeAdvanceWasNotInProgress,
			RTI::FederateInternalError) ;

    void startRegistrationForObjectClass(RTI::ObjectClassHandle theClass)
        throw(RTI::ObjectClassNotPublished, RTI::FederateInternalError);

    void stopRegistrationForObjectClass(RTI::ObjectClassHandle theClass)
        throw(RTI::ObjectClassNotPublished, RTI::FederateInternalError);

    void turnInteractionsOn(RTI::InteractionClassHandle theHandle) 
        throw(RTI::InteractionClassNotPublished, RTI::FederateInternalError);

    void turnInteractionsOff(RTI::InteractionClassHandle theHandle) 
        throw(RTI::InteractionClassNotPublished, RTI::FederateInternalError);

    // Callback : discover object instance
		void discoverObjectInstance ( RTI::ObjectHandle theObject
									, RTI::ObjectClassHandle theObjectClass
									, const char *theObjectName
									)
		throw ( RTI::CouldNotDiscover,
			RTI::ObjectClassNotKnown,
			RTI::FederateInternalError);

    void reflectAttributeValues(RTI::ObjectHandle theObject, 
                                const RTI::AttributeHandleValuePairSet& theAttributes,
                                const RTI::FedTime& theTime, const char *theTag, 
                                RTI::EventRetractionHandle theHandle) 
        throw(RTI::ObjectNotKnown, 
              RTI::AttributeNotKnown, 
              RTI::InvalidFederationTime, 
              RTI::FederateInternalError);

    void receiveInteraction(RTI::InteractionClassHandle theInteraction, 
                            const RTI::ParameterHandleValuePairSet& theParameters, 
                            const RTI::FedTime& theTime, 
                            const char *theTag, 
                            RTI::EventRetractionHandle theHandle) 
        throw(RTI::InteractionClassNotKnown, 
              RTI::InteractionParameterNotKnown, 
              RTI::InvalidFederationTime, 
              RTI::FederateInternalError);

    //   void removeObjectInstance(ObjectHandle theObject, 
    // 			    const FedTime& theTime, 
    // 			    const char *theTag, 
    // 			    EventRetractionHandle theHandle) 
    //     throw(ObjectNotKnown, 
    // 	  InvalidFederationTime, 
    // 	  RTI::FederateInternalError);

    void timeRegulationEnabled(const RTI::FedTime& theTime) 
        throw(RTI::InvalidFederationTime, 
              RTI::EnableTimeRegulationWasNotPending,
              RTI::FederateInternalError);

    RTI::Boolean isAttributeOwnedByFederate(RTI::ObjectHandle theObject,     
                                       RTI::AttributeHandle theAttribute)
        throw(RTI::ObjectNotKnown,
              RTI::AttributeNotDefined,
              RTI::FederateNotExecutionMember,
              RTI::ConcurrentAccessAttempted,
              RTI::SaveInProgress,
              RTI::RestoreInProgress,
              RTI::RTIinternalError);
	
    void attributeIsNotOwned(RTI::ObjectHandle theObject, RTI::AttributeHandle theAttribute)
        throw(RTI::ObjectNotKnown, RTI::AttributeNotKnown, RTI::FederateInternalError);

    void informAttributeOwnership(RTI::ObjectHandle theObject,    
                                       RTI::AttributeHandle theAttribute, 
                                       RTI::FederateHandle theOwner)     
        throw(RTI::ObjectNotKnown, RTI::AttributeNotKnown, RTI::FederateInternalError);

    void attributeOwnershipUnavailable(RTI::ObjectHandle theObject,         
                                       const RTI::AttributeHandleSet& offeredAttributes)
        throw(RTI::ObjectNotKnown,
              RTI::AttributeNotKnown,	
              RTI::AttributeAlreadyOwned,
              RTI::AttributeAcquisitionWasNotRequested,
              RTI::FederateInternalError);	

    void attributeOwnershipAcquisitionNotification(
                                                   RTI::ObjectHandle theObject, 
                                                   const RTI::AttributeHandleSet& securedAttributes) 
        throw(RTI::ObjectNotKnown,
              RTI::AttributeNotKnown,
              RTI::AttributeAcquisitionWasNotRequested,
              RTI::AttributeAlreadyOwned,
              RTI::AttributeNotPublished,		
              RTI::FederateInternalError);	
 
    void requestAttributeOwnershipAssumption(
                                             RTI::ObjectHandle theObject,         
                                             const RTI::AttributeHandleSet& offeredAttributes, 
                                             const char *theTag)            
        throw(RTI::ObjectNotKnown,
              RTI::AttributeNotKnown,
              RTI::AttributeAlreadyOwned,
              RTI::AttributeNotPublished,
              RTI::FederateInternalError);
	
    void attributeOwnershipDivestitureNotification (
                                                    RTI::ObjectHandle theObject,
                                                    const RTI::AttributeHandleSet& releasedAttributes) 
        throw(RTI::ObjectNotKnown,
              RTI::AttributeNotKnown,
              RTI::AttributeNotOwned,	
              RTI::AttributeDivestitureWasNotRequested,	
              RTI::FederateInternalError);

    void requestAttributeOwnershipRelease (
                                           RTI::ObjectHandle theObject,
                                           const RTI::AttributeHandleSet& candidateAttributes,
                                           const char *theTag) 
        throw(RTI::ObjectNotKnown,
              RTI::AttributeNotKnown,
              RTI::AttributeNotOwned,
              RTI::FederateInternalError);
	
    void confirmAttributeOwnershipAcquisitionCancellation (
                                                           RTI::ObjectHandle theObject,
                                                           const RTI::AttributeHandleSet& theAttributes)
        throw(RTI::ObjectNotKnown,
              RTI::AttributeNotKnown,
              RTI::AttributeAlreadyOwned,
              RTI::AttributeAcquisitionWasNotCanceled,
              RTI::FederateInternalError);		


    // ------ temporaire ---------------------------------------------------------

    virtual void setTAG(bool);  // Time Advance Granted
    virtual bool getTAG(void);
    void setId(int);

};
#endif // FED_HH


