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


#ifndef ENTITY_HH
#define ENTITY_HH

#include <string>
#include <iostream>

using namespace std ;

template<typename H>
class Entity {

    // Attributes
protected:
    H handle ;
    string name ;
    vector<H> tr ;

    // Methods
public:
    Entity(string);
    Entity(string, H);
    ~Entity();  

    string getName();
    H getHandle();
    void setHandle(H);
    void addTranslation(H);
    H getTranslation(int);

    virtual void dump(void);
};

// --------------------------------------------------------------------------

template<typename H>
Entity<H>::Entity(string s)
{
    handle = 0 ;
    name = s ;
}

template<typename H>
Entity<H>::Entity(string s, H h)
{
    handle = h ;
    name = s ;
}

template<typename H>
Entity<H>::~Entity(void)
{
  
}

template<typename H>
string 
Entity<H>::getName(void) { 

    return name ;
}

template<typename H>
H 
Entity<H>::getHandle(void) 
{ 
    return handle ;
} 

template<typename H>
void
Entity<H>::setHandle(H h) 
{
    handle = h ;
}

template<typename H>
void
Entity<H>::dump(void)
{
    cout << "[" << handle << "|" << name << "|( " ;
    for(int i=0; i<tr.size(); i++) cout << tr[i] << " " ;
    cout << ")]" << endl ;
}

template<typename H>
void
Entity<H>::addTranslation(H h)
{
    tr.push_back(h);
}

template<typename H>
H
Entity<H>::getTranslation(int i)
{
    if(i>=0 && i<tr.size()) return tr[i] ;
    else return 0 ;
}

#endif // ENTITY_HH
