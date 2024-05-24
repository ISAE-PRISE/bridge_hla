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


#ifndef CONTAINER_ENTITY_HH
#define CONTAINER_ENTITY_HH

#include "Entity.hh"
#include <vector>

template<typename H, class A>
class ContainerEntity : public Entity<H>
{
protected:
    vector<A> attributes ;
    vector<ContainerEntity<H, A> > subEntities ;

    // Methods
public:
    ContainerEntity(string s);
    ~ContainerEntity();

    void addAttribute(string a);
    void addSubEntity(string e);

    vector<A>& getAttributes();
    vector<ContainerEntity<H, A> >& getSubEntities();

    virtual void dump(void);
};

// ---------------------------------------------------------------------------
// ContainerEntity
// 
template<typename H, class A>
ContainerEntity<H, A>::ContainerEntity(string s) : Entity<H>(s) 
{
    //name = s ;
    //handle = 0 ;
}

// ---------------------------------------------------------------------------
// ~ContainerEntity
// 
template<typename H, class A>
ContainerEntity<H, A>::~ContainerEntity()
{
    attributes.clear();
    subEntities.clear();
}

template<typename H, class A>
void
ContainerEntity<H, A>::addAttribute(string a)
{
    attributes.push_back(A(a)) ;
}

template<typename H, class A>
void
ContainerEntity<H, A>::addSubEntity(string e)
{
    subEntities.push_back(ContainerEntity<H, A>(e)) ;
}

template<typename H, class A>
vector<A>&
ContainerEntity<H, A>::getAttributes()
{
    return attributes ;
}

template<typename H, class A>
vector<ContainerEntity<H, A> >&
ContainerEntity<H, A>::getSubEntities()
{
    return subEntities ;
}

template<typename H, class A>
void
ContainerEntity<H, A>::dump(void)
{
    //cout << "[" << handle << "|" << name << "|( " ;
    //for(int i=0; i<tr.size(); i++) cout << tr[i] << " " ;
    //cout << ")] -->" << endl ;
    for(typename vector<A>::iterator i=attributes.begin(); i!=attributes.end(); i++) 
        i->dump();
    //cout << "-----" << endl ;
    for(typename vector<ContainerEntity<H, A> >::iterator i=subEntities.begin();
        i!=subEntities.end(); i++) i->dump();
    //cout << "<--" << endl ;
}

#endif // CONTAINER_ENTITY_HH
