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

#include "Fed.hh"

Fed::Fed(RTI::RTIambassador *rtia, 
         Federate* federate_, 
         Federation* federation_)
{
    this->rtiamb = rtia ;
    this->granted = false ;
    federate = federate_ ;
    federation = federation_ ;
    verbose = false ;
    id = -1 ;
}

Fed::~Fed()
{ 

}

// ===========================================================================
// 1. FEDERATION MANAGEMENT
// ===========================================================================

// ---------------------------------------------------------------------------
// 1.1 announceSynchronizationPoint
// ---------------------------------------------------------------------------
void 
Fed::announceSynchronizationPoint(const char* label_, const char* tag_)
    throw(RTI::FederateInternalError)
{
    if (verbose) {
        cout << "Callback(" << id << ") - Announce Synchronization Point : " 
             << label_ << endl ;
    }
    federate->announce(label_);
}

// FIXME: 1.2 federationNotRestored
// FIXME: 1.3 federationNotSaved
// FIXME: 1.4 federationRestoreBegun
// FIXME: 1.5 federationRestored
// FIXME: 1.6 federationSaved

// ---------------------------------------------------------------------------
// 1.7 federationSynchronized
// ---------------------------------------------------------------------------
void 
Fed::federationSynchronized(const char *label_)
    throw(RTI::FederateInternalError)
{ 
    federate->endSynchronization(label_);
}

// FIXME: 1.8 initiateFederateRestore
// FIXME: 1.9 initiateFederateSave
// FIXME: 1.10 requestFederationRestoreFailed
// FIXME: 1.11 requestFederationRestoreSucceeded

// ---------------------------------------------------------------------------
// 1.12 synchronisationPointRegistrationFailed
// ---------------------------------------------------------------------------
void 
Fed::synchronizationPointRegistrationFailed(const char *label_) 
    throw (RTI::FederateInternalError)
{
}

// ---------------------------------------------------------------------------
// 1.13 synchronisationPointRegistrationSucceeded
// ---------------------------------------------------------------------------

void 
Fed::synchronizationPointRegistrationSucceeded(const char *label_) 
    throw(RTI::FederateInternalError)
{
 
}

// ===========================================================================
// 2. DECLARATION MANAGEMENT
// ===========================================================================

// ---------------------------------------------------------------------------
// 2.1 startRegistrationForObjectClass
// ---------------------------------------------------------------------------
void 
Fed::startRegistrationForObjectClass(RTI::ObjectClassHandle theClass)
    throw (RTI::ObjectClassNotPublished, RTI::FederateInternalError)
{
}

// ---------------------------------------------------------------------------
// 2.2 stopRegistrationForObjectClass
// ---------------------------------------------------------------------------
void 
Fed::stopRegistrationForObjectClass(RTI::ObjectClassHandle theClass)
    throw (RTI::ObjectClassNotPublished, RTI::FederateInternalError)
{
}

// ---------------------------------------------------------------------------
// 2.3 turnInteractionsOff
// ---------------------------------------------------------------------------
void Fed::turnInteractionsOff(RTI::InteractionClassHandle theHandle) 
    throw (RTI::InteractionClassNotPublished, RTI::FederateInternalError)
{
}

// ---------------------------------------------------------------------------
// 2.4 turnInteractionsOn
// ---------------------------------------------------------------------------
void
Fed::turnInteractionsOn(RTI::InteractionClassHandle theHandle) 
    throw (RTI::InteractionClassNotPublished, RTI::FederateInternalError)
{

}

// ===========================================================================
// 3. OBJECT MANAGEMENT
// ===========================================================================

// FIXME: 3.1 attributesInScope
// FIXME: 3.2 attributesOutOfScope

// ---------------------------------------------------------------------------
// 3.3 discoverObjectInstance
// ---------------------------------------------------------------------------
void
Fed::discoverObjectInstance(RTI::ObjectHandle handle,
                            RTI::ObjectClassHandle class_handle, 
                            const char *name)
    throw (RTI::CouldNotDiscover, RTI::ObjectClassNotKnown,
           RTI::FederateInternalError)
{ 
    if (verbose) {
        cout << "Callback(" << id << ") - Discover Object Instance, "
             << "handle " << handle << ", class " << class_handle << ", name "
             << name << endl ;
    }

    federate->discoverObject(handle, class_handle, name);
}

// FIXME: 3.4 provideAttributeValueUpdate

// ---------------------------------------------------------------------------
// 3.5 receiveInteraction
// ---------------------------------------------------------------------------
void 
Fed::receiveInteraction(RTI::InteractionClassHandle interaction, 
                        const RTI::ParameterHandleValuePairSet& parameters, 
                        const RTI::FedTime& time, 
                        const char *tag, 
                        RTI::EventRetractionHandle theHandle) 
    throw (RTI::InteractionClassNotKnown, RTI::InteractionParameterNotKnown,
           RTI::InvalidFederationTime, RTI::FederateInternalError)
{
    if (verbose) {
    cout << "Callback(" << id << ") - Receive Interaction " 
         << interaction << endl ;
    }

    federate->receive(interaction, parameters, time);
}

// ---------------------------------------------------------------------------
// 3.6 reflectAttributeValues
// ---------------------------------------------------------------------------
void 
Fed::reflectAttributeValues(RTI::ObjectHandle object,
                            const RTI::AttributeHandleValuePairSet& attributes,
                            const RTI::FedTime& time, 
                            const char *tag, 
                            RTI::EventRetractionHandle theHandle) 
    throw (RTI::ObjectNotKnown, RTI::AttributeNotKnown, RTI::InvalidFederationTime,
           RTI::FederateInternalError)
{
    if (verbose) {
        cout << "Callback(" << id << ") - Reflect Attribute Values "
             << object << endl ;
    }

    federate->reflect(object, attributes, time);
}

// --------------------------------------------------------------------------
// 3.7 removeObjectInstance
// --------------------------------------------------------------------------
void
Fed::removeObjectInstance(RTI::ObjectHandle object, 
                          const RTI::FedTime& time, 
                          const char *tag, 
                          RTI::EventRetractionHandle theHandle) 
    throw (RTI::ObjectNotKnown, RTI::InvalidFederationTime, RTI::FederateInternalError)
{
    if (verbose) {
        cout << "Callback(" << id << ") - Remove Object Instance " 
             << object << endl ;
    }
    federate->removeObject(object, time);
}

// FIXME: 3.8 turnUpdatesOffForObjectInstance 
// FIXME: 3.9 turnUpdatesOnForObjectInstance 

// ===========================================================================
// 4. OWNERSHIP MANAGEMENT
// ===========================================================================

// ---------------------------------------------------------------------------
// 4.1 attributeIsNotOwned
// ---------------------------------------------------------------------------
void 
Fed::attributeIsNotOwned(RTI::ObjectHandle theObject, RTI::AttributeHandle theAttribute)
    throw (RTI::ObjectNotKnown, RTI::AttributeNotKnown, RTI::FederateInternalError)
{
}

// FIXME: 4.2 attributeOwnedByRTI

// ---------------------------------------------------------------------------
// 4.3 attributeOwnershipAcquisitionNotification
// ---------------------------------------------------------------------------
void 
Fed::attributeOwnershipAcquisitionNotification(RTI::ObjectHandle theObject,
                                               const RTI::AttributeHandleSet& securedAttributes)
    throw (RTI::ObjectNotKnown, RTI::AttributeNotKnown, 
           RTI::AttributeAcquisitionWasNotRequested, RTI::AttributeAlreadyOwned,
           RTI::AttributeNotPublished, RTI::FederateInternalError)
{
}

// ---------------------------------------------------------------------------
// 4.4 attributeOwnershipDivestitureNotification
// ---------------------------------------------------------------------------
void 
Fed::attributeOwnershipDivestitureNotification(RTI::ObjectHandle theObject, 
                                               const RTI::AttributeHandleSet& releasedAttributes) 
    throw (RTI::ObjectNotKnown, RTI::AttributeNotKnown, RTI::AttributeNotOwned,
           RTI::AttributeDivestitureWasNotRequested, RTI::FederateInternalError)
{
}

// ---------------------------------------------------------------------------
// 4.5 attributeOwnershipUnavailable
// ---------------------------------------------------------------------------
void 
Fed::attributeOwnershipUnavailable(RTI::ObjectHandle theObject, 
                                   const RTI::AttributeHandleSet& offeredAttributes)
  throw(RTI::ObjectNotKnown,
              RTI::AttributeNotKnown,	
              RTI::AttributeAlreadyOwned,
              RTI::AttributeAcquisitionWasNotRequested,
              RTI::FederateInternalError)
{
} 

// ---------------------------------------------------------------------------
// 4.6 confirmAttributeOwnershipAcquisitionCancellation
// ---------------------------------------------------------------------------
void Fed::confirmAttributeOwnershipAcquisitionCancellation(
                                                           RTI::ObjectHandle theObject,
                                                           const RTI::AttributeHandleSet& theAttributes)
    throw (RTI::ObjectNotKnown, RTI::AttributeNotKnown, RTI::AttributeAlreadyOwned,
           RTI::AttributeAcquisitionWasNotCanceled, RTI::FederateInternalError)
{
}

// ---------------------------------------------------------------------------
// 4.7 informAttributeOwnership
// ---------------------------------------------------------------------------
void Fed::informAttributeOwnership(RTI::ObjectHandle theObject,
                                   RTI::AttributeHandle theAttribute,
                                   RTI::FederateHandle theOwner)
    throw (RTI::ObjectNotKnown, RTI::AttributeNotKnown, RTI::FederateInternalError)
{
}

// ---------------------------------------------------------------------------
// 4.8 requestAttributeOwnershipAssumption
// ---------------------------------------------------------------------------
void
Fed::requestAttributeOwnershipAssumption(RTI::ObjectHandle theObject,
                                         const RTI::AttributeHandleSet& offeredAttributes,
                                         const char *theTag)
    throw (RTI::ObjectNotKnown, RTI::AttributeNotKnown, RTI::AttributeAlreadyOwned, 
           RTI::AttributeNotPublished, RTI::FederateInternalError)
{
}

// ---------------------------------------------------------------------------
// 4.9 requestAttributeOwnershipRelease
// ---------------------------------------------------------------------------
void 
Fed::requestAttributeOwnershipRelease(RTI::ObjectHandle theObject,
                                      const RTI::AttributeHandleSet& candidateAttributes,
                                      const char *theTag) 
    throw (RTI::ObjectNotKnown, RTI::AttributeNotKnown, RTI::AttributeNotOwned, 
           RTI::FederateInternalError)
{
}

// ===========================================================================
// 5. TIME MANAGEMENT
// ===========================================================================

// FIXME: 5.1 requestRetraction

// ---------------------------------------------------------------------------
// 5.2 timeAdvanceGrant
// ---------------------------------------------------------------------------
void 
Fed::timeAdvanceGrant(const RTI::FedTime& theTime) 
    throw (RTI::InvalidFederationTime, RTI::TimeAdvanceWasNotInProgress, 
           RTI::FederateInternalError) 
{
    granted = true ;
}

// FIXME: 5.3 timeConstrainedEnabled

// ---------------------------------------------------------------------------
// 5.4 timeRegulationEnabled
// ---------------------------------------------------------------------------
void 
Fed::timeRegulationEnabled(const RTI::FedTime& theTime) 
    throw(RTI::InvalidFederationTime, RTI::EnableTimeRegulationWasNotPending,
          RTI::FederateInternalError) 
{

}

// TEMPORAIRE

void
Fed::setTAG(bool granted_) {

    this->granted = granted_ ;
}

bool
Fed::getTAG(void) {

    return granted ;
}

void
Fed::setId(int i) {
    id = i ;
}

// ---------------------------------------------------------------------------
// setVerbose
//
void
Fed::setVerbose(bool v)
{
    verbose = v ;
}


