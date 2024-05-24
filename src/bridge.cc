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

#include <config.h>

#include "Fed.hh"
#include "Federate.hh"

#include "cmdline.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <RTI.hh>

#include <cstdio>
#include <vector>
#include <string>
#include <signal.h>
#include <csignal>
#include <unistd.h>

using std::string ;

extern "C" void HandleSignal(int);
static void ProcessXmlNode(xmlDocPtr, xmlNodePtr, const char *, string&);
bool stop = false ;

// ---------------------------------------------------------------------------
// main
// 
int 
main(int argc, char** argv)
{
    int id = 1 ;
    vector<Federate*> feds ;
    char* configfile ;
    xmlDocPtr doc ;
    xmlNodePtr cur ;
    xmlNodePtr fed ;

    gengetopt_args_info args_info;
    if(cmdline_parser(argc, argv, &args_info) != 0) exit(1) ;

    std::signal(SIGINT, HandleSignal);
    cout << PACKAGE << "-" << VERSION << endl ;

    configfile = args_info.config_arg ;
    doc = xmlParseFile(configfile);
    cur = xmlDocGetRootElement(doc);
    if(cur==NULL) {
        cout << "Error: empty configuration file" << endl ;
        xmlFreeDoc(doc);
        exit(-1);
    }
    if(xmlStrcmp(cur->name, (const xmlChar*) "interfederation")) {
        cout << "Error: wrong type, no interfederation found" << endl ;
        xmlFreeDoc(doc);
    }
    cur = cur->xmlChildrenNode ;
    while(cur != NULL) {
        if((!xmlStrcmp(cur->name, (const xmlChar*) "federation"))) {
            cout << "Federation (" ;

            string federation ;
            string federate ;
            string fedfile ;
            string host ;
            string synchro ;
            string filter ;

            fed = cur->xmlChildrenNode ;

            while (fed != NULL) {
                ProcessXmlNode(doc, fed, "name", federation);
                ProcessXmlNode(doc, fed, "host", host);
                ProcessXmlNode(doc, fed, "filter", filter);
                ProcessXmlNode(doc, fed, "surrogate", federate);
                ProcessXmlNode(doc, fed, "file", fedfile);
                ProcessXmlNode(doc, fed, "synchro", synchro);
                fed = fed->next ;
            }      
            if (federation.empty() || federate.empty() || fedfile.empty() || 
                host.empty()) {
                cout << "Error: missing information for a federation" << endl ;
                xmlFreeDoc(doc);
                exit(1);
            }

            cout << federation << "/" << federate << "/" << fedfile << ")" 
                 << endl ;
            Federate *f = new Federate(federation, federate, fedfile, host, 
                                       filter);
            f->setId(id++);
            f->setVerbose(args_info.verbose_flag);
            if (synchro != "") {
                cout << "(synchro: " << synchro << ")" << endl ;
                f->setSynchro(synchro);
            }
            feds.push_back(f);
        }
        cur=cur->next;
    }
  
    xmlFreeDoc(doc);

    cout << "Bridge - Joining federations" << endl ;
    bool joined = false ;
    while (!joined) {
        joined = true ;
        for(vector<Federate*>::iterator i=feds.begin(); i!=feds.end(); i++) {
            if ((*i)->join()) {
                joined = false ;
            }
        } 
    }

    cout << "Bridge - Connecting bridge federates" << endl ;
    for(vector<Federate*>::iterator i=feds.begin(); i!=feds.end(); i++) {
        for(vector<Federate*>::iterator j=feds.begin(); j!=feds.end(); j++) {
            if(i!=j) (*i)->connect(**j);
        }
    }

    cout << "Bridge - Initializing bridge federates" << endl ;
    for(vector<Federate*>::iterator i=feds.begin(); i!=feds.end(); i++) {
        (*i)->init();
    }

    struct sigaction a ;
    if(args_info.timer_given) {
        int timer = args_info.timer_arg ;
        printf("Timer     : %3ds set\n", timer);
        a.sa_handler = HandleSignal  ;
        sigemptyset(&a.sa_mask); 
        sigaction(SIGALRM, &a, NULL);
        alarm(timer);
    } else {
        cout << "Bridge - Running... (press Ctrl-C to stop)" << endl ;
    }

    while(!stop) {
        for(vector<Federate*>::iterator i=feds.begin(); i!=feds.end(); i++) {
            (*i)->step();
        }
    }

    for(vector<Federate*>::iterator i=feds.begin(); i!=feds.end(); i++) {
        delete *i ;
    } 
    feds.clear();

    sleep(3); // laisser les infos du RTIA sortir sur stdout...

    cout << "Bridge - Exiting." << endl ;
}

// ---------------------------------------------------------------------------
// HandleSignal
// 
void 
HandleSignal(int signum)
{
    if((signum==SIGINT) || (signum=SIGALRM)) {
        stop = true ;
    }
}

// ---------------------------------------------------------------------------
void
ProcessXmlNode(xmlDocPtr doc, xmlNodePtr node, const char *name, string &s)
{
    if ((!xmlStrcmp(node->name, (const xmlChar*) name))) {
        s = string((const char *) 
                   xmlNodeListGetString(doc, node->xmlChildrenNode, 1));
    }
}
