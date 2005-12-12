// @(#)root/cintex:$Name:  $:$Id: Cintex.cxx,v 1.5 2005/12/07 21:33:59 roiser Exp $
// Author: Pere Mato 2005

// Copyright CERN, CH-1211 Geneva 23, 2004-2005, All rights reserved.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.

#include "Reflex/Callback.h"
#include "Reflex/Type.h"
#include "Reflex/Member.h"
#include "Reflex/Member.h"
#include "Reflex/Builder/ReflexBuilder.h"

#include "Cintex/Cintex.h"
#include "CINTClassBuilder.h"
#include "CINTFunctionBuilder.h"
#include "CINTVariableBuilder.h"
#include "CINTTypedefBuilder.h"
#include "CINTEnumBuilder.h"
#include "ROOTClassEnhancer.h"
#include <iostream>

using namespace ROOT::Reflex;
using namespace ROOT::Cintex;
using namespace std;

//Build the Cintex dictionary on Reflex and convert it to CINT
namespace {
  struct Cintex_dict { 
    public:
      Cintex_dict() {
        //--Reflex class builder
        //NamespaceBuilder( "ROOT::Cintex" );
        Type t_void = TypeBuilder("void");
        Type t_int  = TypeBuilder("int");
        ClassBuilderT< Cintex >("Cintex", PUBLIC)
         .AddFunctionMember(FunctionTypeBuilder(t_void), "Enable", Enable, 0, 0, PUBLIC | STATIC)
         .AddFunctionMember(FunctionTypeBuilder(t_void, t_int), "SetDebug", SetDebug, 0, 0, PUBLIC | STATIC)
         .AddFunctionMember(FunctionTypeBuilder(t_int), "Debug", Debug, 0, 0, PUBLIC | STATIC);
        //--CINT class builder
	      Type t = Type::ByName("Cintex");
        ROOT::Cintex::CINTClassBuilder::Get(t).Setup();
      }
      static void* Enable(void*, const std::vector<void*>&, void*) {
        Cintex::Enable();
        return 0;
      }
      static void* SetDebug(void*, const std::vector<void*>& arg, void*) {
        Cintex::SetDebug(*(bool*)arg[0]);
        return 0;
      }
      static void* Debug(void*, const std::vector<void*>&, void*) {
        static int b = Cintex::Debug();
        return &b;
      }

  };
  static Cintex_dict s_dict;
}


namespace ROOT {
  namespace Cintex {
    
  Cintex& Cintex::Instance() {
    static Cintex s_instance;
    return s_instance;
  }

  Cintex::Cintex() {
    fCallback = new Callback();
    fRootcreator = 0;
    fDbglevel = 0;
    fEnabled = false;
  }

  Cintex::~Cintex() {
    if( fCallback ) UninstallClassCallback( fCallback );
    delete fCallback;
  }

  void Cintex::Enable() {
    if ( Instance().fEnabled ) return;
    //---Install the callback to fothcoming classes ----//
    InstallClassCallback( Instance().fCallback );        
    //---Convert to CINT all existing classes ---//
    for( size_t i = 0; i < Type::TypeSize(); i++ ) {

      ( * Instance().fCallback)( Type::TypeAt(i) );
    }
    //---Convert to CINT all existing free functions and variables
    for ( size_t n = 0; n < Scope::ScopeSize(); n++ ) {
      Scope ns = Scope::ScopeAt(n);
      if ( ns.IsNamespace() ) {
        for( size_t m = 0; m < ns.MemberSize(); m++ ) {
          ( * Instance().fCallback)( ns.MemberAt(m) );
        }
      }
    }
    Instance().fEnabled = true;
  } 

  void Cintex::SetROOTCreator(ROOTCreator c) {
    Instance().fRootcreator = c;
  }

  ROOTCreator Cintex::GetROOTCreator() {
    return Instance().fRootcreator;
  }

  int Cintex::Debug() {
    return Instance().fDbglevel;
  }

  void Cintex::SetDebug(int l) {
    Instance().fDbglevel = l;
  }
  
  void Callback::operator () ( const Type& t ) {
    if ( t.IsClass() || t.IsStruct() ) {
      ROOTClassEnhancer enhancer(t);
      enhancer.Setup();
      CINTClassBuilder::Get(t).Setup();
      enhancer.CreateInfo();
    }
    else if ( t.IsTypedef() ) {
      CINTTypedefBuilder::Setup(t);
    }
    else if ( t.IsEnum() ) {
      CINTEnumBuilder::Setup(t);
    } 
  }
  
  void Callback::operator () ( const Member& m ) {
    if ( m.IsFunctionMember() ) {
      if( Cintex::Debug() ) cout << "Building function " << m.Name(SCOPED|QUALIFIED) << endl; 
      CINTFunctionBuilder(m).Setup();
    }
    else if ( m.IsDataMember() ) {
      if( Cintex::Debug() ) cout << "Building variable " << m.Name(SCOPED|QUALIFIED) << endl; 
      CINTVariableBuilder(m).Setup();
    } 
  }
}}
