#include <designators/CDesignator.h>


CDesignator::CDesignator() : CKeyValuePair() {
  this->m_edtType = UNKNOWN;
}

CDesignator::CDesignator(designator_integration_msgs::Designator desigContent) {
  this->fillFromDesignatorMsg(desigContent);
}

void CDesignator::setType(enum DesignatorType edtType) {
  m_edtType = edtType;
}

void CDesignator::fillFromDesignatorMsg(designator_integration_msgs::Designator desigContent) {
  m_edtType = (enum DesignatorType)desigContent.type;
  
  list<CKeyValuePair*> lstKVPs;
  for(vector<designator_integration_msgs::KeyValuePair>::iterator itKVP = desigContent.description.begin();
      itKVP != desigContent.description.end();
      itKVP++) {
    designator_integration_msgs::KeyValuePair kvpCurrent = *itKVP;
    
    CKeyValuePair *ckvpCurrent = new CKeyValuePair(kvpCurrent);
    lstKVPs.push_back(ckvpCurrent);
  }
  
  while(lstKVPs.size() > 0) {
    for(list<CKeyValuePair*>::iterator itKVP = lstKVPs.begin();
	itKVP != lstKVPs.end();
	itKVP++) {
      CKeyValuePair *kvpCurrent = *itKVP;
      bool bFoundChild = false;
      
      for(list<CKeyValuePair*>::iterator itKVPInspect = lstKVPs.begin();
	  itKVPInspect != lstKVPs.end();
	  itKVPInspect++) {
	CKeyValuePair *kvpInspect = *itKVPInspect;
	
	if(kvpInspect != kvpCurrent && kvpInspect->parent() == kvpCurrent->id()) {
	  bFoundChild = true;
	  break;
	}
      }
      
      if(!bFoundChild) {
	bool bFoundParent = false;
	
	for(list<CKeyValuePair*>::iterator itKVPInspect = lstKVPs.begin();
	    itKVPInspect != lstKVPs.end();
	    itKVPInspect++) {
	  CKeyValuePair *kvpInspect = *itKVPInspect;
	  
	  if(kvpInspect != kvpCurrent && kvpInspect->id() == kvpCurrent->parent()) {
	    kvpInspect->addChild(kvpCurrent);
	    lstKVPs.remove(kvpCurrent);
	    bFoundParent = true;
	    break;
	  }
	}
	
	if(bFoundParent) {
	  break;
	} else {
	  m_lstChildren.push_back(kvpCurrent);
	  lstKVPs.remove(kvpCurrent);
	  break;
	}
      }
    }
  }
}

enum DesignatorType CDesignator::type() {
  return m_edtType;
}

void CDesignator::printDesignator() {
  for(list<CKeyValuePair*>::iterator itKVP = m_lstChildren.begin();
      itKVP != m_lstChildren.end();
      itKVP++) {
    CKeyValuePair *kvpCurrent = *itKVP;
    
    kvpCurrent->printPair(0);
  }
}

designator_integration_msgs::Designator CDesignator::serializeToMessage() {
  designator_integration_msgs::Designator msgDesig;
  msgDesig.type = (int)m_edtType;
  
  int nHighestID = 0;
  vector<designator_integration_msgs::KeyValuePair> vecKVPs;
  for(list<CKeyValuePair*>::iterator itKVP = m_lstChildren.begin();
      itKVP != m_lstChildren.end();
      itKVP++) {
    CKeyValuePair *ckvpCurrent = *itKVP;
    
    vector<designator_integration_msgs::KeyValuePair> vecKVPMsgs = ckvpCurrent->serializeToMessage(0, nHighestID + 1);
    for(vector<designator_integration_msgs::KeyValuePair>::iterator itKVPMsg = vecKVPMsgs.begin();
	itKVPMsg != vecKVPMsgs.end();
	itKVPMsg++) {
      designator_integration_msgs::KeyValuePair kvpCurrent = *itKVPMsg;
      
      if(kvpCurrent.id > nHighestID) {
	nHighestID = kvpCurrent.id;
      }
      
      vecKVPs.push_back(kvpCurrent);
    }
  }
  
  msgDesig.description = vecKVPs;
  
  return msgDesig;
}
