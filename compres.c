/*
 * A n t l r  T r a n s l a t i o n  H e a d e r
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * With AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR33
 *
 *   antlr -gt compres.g
 *
 */

#define ANTLR_VERSION	13333
#include "pcctscfg.h"
#include "pccts_stdio.h"

#include <string>
#include <iostream>
#include "stdlib.h"
#include <map>
using namespace std;

// struct to store information about tokens
typedef struct {
  string kind;
  string text;
} Attrib;


// function to fill token information (predeclaration)
void zzcr_attr(Attrib *attr, int type, char *text);

// fields for AST nodes
#define AST_FIELDS string kind; string text;
#include "ast.h"

// macro to create a new AST node (and function predeclaration)
#define zzcr_ast(as,attr,ttype,textt) as=createASTnode(attr,ttype,textt)
AST* createASTnode(Attrib* attr, int ttype, char *textt);
#define GENAST

#include "ast.h"

#define zzSET_SIZE 4
#include "antlr.h"
#include "tokens.h"
#include "dlgdef.h"
#include "mode.h"

/* MR23 In order to remove calls to PURIFY use the antlr -nopurify option */

#ifndef PCCTS_PURIFY
#define PCCTS_PURIFY(r,s) memset((char *) &(r),'\0',(s));
#endif

#include "ast.c"
zzASTgvars

ANTLR_INFO

#include <cstdlib>
#include <cmath>

AST *root;

typedef map<string, int>quantProductes;
map<string, quantProductes>llistaCompra;

// function to fill token information
void zzcr_attr(Attrib *attr, int type, char *text) {
  if (type == ID) {
    attr->kind = "id";
    attr->text = text;
  }
  else {
    attr->kind = text;
    attr->text = "";
  }
}

// function to create a new AST node
AST* createASTnode(Attrib* attr, int type, char* text) {
  AST* as = new AST;
  as->kind = attr->kind;
  as->text = attr->text;
  as->right = NULL;
  as->down = NULL;
  return as;
}


/// create a new "list" AST node with one element
AST* createASTlist(AST *child) {
  AST *as=new AST;
  as->kind="list";
  as->right=NULL;
  as->down=child;
  return as;
}

//donat un nom de compra, retorna l'arbre on esta definida
AST *findASTCompraDef(string id) {
AST *n = root->down;
while (n != NULL and (n->kind != "=" or n->down->text != id)) n = n->right;
if (id != n->down->text) {cout << "MISMATCH: " << id << " " << n->down->text << endl;}
return n;
}

/// get nth child of a tree. Count starts at 0.
/// if no such child, returns NULL
AST* child(AST *a,int n) {
AST *c=a->down;
for (int i=0; c!=NULL && i<n; i++) c=c->right;
return c;
}

/// print AST, recursively, with indentation
void ASTPrintIndent(AST *a,string s)
{
if (a==NULL) return;

  cout<<a->kind;
if (a->text!="") cout<<"("<<a->text<<")";
cout<<endl;

  AST *i = a->down;
while (i!=NULL && i->right!=NULL) {
cout<<s+"  \\__";
ASTPrintIndent(i,s+"  |"+string(i->kind.size()+i->text.size(),' '));
i=i->right;
}

  if (i!=NULL) {
cout<<s+"  \\__";
ASTPrintIndent(i,s+"   "+string(i->kind.size()+i->text.size(),' '));
i=i->right;
}
}

/// print AST
void ASTPrint(AST *a)
{
while (a!=NULL) {
cout<<" ";
ASTPrintIndent(a,"");
a=a->right;
}
}

//-------------------------------------------------------------------------------------------
//EL MEU CODI
int unitats(AST *a, bool imp) {
if (imp) cout << endl << "en nombre d'unitats de la " << a->text  << " es: ";
map<string, quantProductes>::iterator i = llistaCompra.find(a->text);
quantProductes sortida = (*i).second;
quantProductes::iterator it = sortida.begin();
int cont = 0;
while (it != sortida.end()) {
if ((*it).second > 0) cont += (*it).second;
++it;
}
if (imp) cout << cont << "." << endl;
return cont;
}

int productes(AST *a, bool imp) {
if (imp) cout << endl << "els productes de la " << a->text  << " son:" << endl;
map<string, quantProductes>::iterator i = llistaCompra.find(a->text);
quantProductes sortida = (*i).second;
quantProductes::iterator it = sortida.begin();
int cont = 0;
while (it != sortida.end()) {
if ((*it).second > 0) {
if (imp) cout << (*it).first << " " << (*it).second << endl;
++cont;
}
++it;
}
return cont;
}

double stdev(AST *a) {
//  valor-mitjana² / nombre total
double mitjana = unitats(a, false) / (double) productes(a, false);
map<string, quantProductes>::iterator i = llistaCompra.find(a->text);
quantProductes sortida = (*i).second;
quantProductes::iterator it = sortida.begin();
double numerador = 0;
while (it != sortida.end()) {
numerador = numerador + pow((*it).second - (double) mitjana, 2.0);
++it;
}
double desv = numerador / (double) productes(a, false);
cout << endl << "la desviació de la " << a->text  << " es: " << desv << endl;
return desv;
}

void assignacio(AST *a, quantProductes& productesCompra) {
productesCompra[a->down->text] = atoi(a->kind.c_str());
if (a->right == NULL) return;
else assignacio(a->right, productesCompra);
}

quantProductes minusOperacio(AST *a, AST *b) {
quantProductes res;
map<string, quantProductes>::iterator it = llistaCompra.find(a->text);
quantProductes productesCompra1 = (*it).second;
it = llistaCompra.find(b->text);
quantProductes productesCompra2 = (*it).second;

  quantProductes::iterator it1 = productesCompra1.begin();
quantProductes::iterator it2 = productesCompra2.begin();

  res.insert(productesCompra1.begin(), productesCompra1.end());
res.insert(productesCompra2.begin(), productesCompra2.end());
if (productesCompra1.size() > productesCompra2.size()) {
while (it1 != productesCompra1.end()) {
it2 = productesCompra2.find((*it1).first);
if (it2 != productesCompra2.end()) {
res[(*it1).first] = (*it1).second - (*it2).second;
}
++it1;
}
}
else {
while (it2 != productesCompra2.end()) {
it1 = productesCompra1.find((*it2).first);
if (it1 != productesCompra1.end()) {
res[(*it2).first] = (*it1).second - (*it2).second;
}
++it2;
}
}
return res;
}

quantProductes minusOperacioRes(AST *a, quantProductes& res) {
map<string, quantProductes>::iterator it = llistaCompra.find(a->text);
quantProductes productesCompra1 = (*it).second;

  quantProductes::iterator it1 = productesCompra1.begin();
quantProductes::iterator it2 = res.begin();

  if (productesCompra1.size() > res.size()) {
while (it1 != productesCompra1.end()) {
it2 = res.find((*it1).first);
if (it2 != res.end()) {
res[(*it1).first] = (*it1).second - (*it2).second;
}
++it1;
}
}
else {
while (it2 != res.end()) {
it1 = productesCompra1.find((*it2).first);
if (it1 != productesCompra1.end()) {
res[(*it2).first] = (*it1).second - (*it2).second;
if (res[(*it2).first] < 0) res[(*it2).first] *= -1;
}
++it2;
}
}
// res.insert(productesCompra1.begin(), productesCompra1.end());
return res;
}

quantProductes andOperacio(AST *a, AST *b) {
quantProductes res;
map<string, quantProductes>::iterator it = llistaCompra.find(a->text);
quantProductes productesCompra1 = (*it).second;
it = llistaCompra.find(b->text);
quantProductes productesCompra2 = (*it).second;

  quantProductes::iterator it1 = productesCompra1.begin();
quantProductes::iterator it2 = productesCompra2.begin();

  res.insert(productesCompra1.begin(), productesCompra1.end());
res.insert(productesCompra2.begin(), productesCompra2.end());
if (productesCompra1.size() > productesCompra2.size()) {
while (it1 != productesCompra1.end()) {
it2 = productesCompra2.find((*it1).first);
if (it2 != productesCompra2.end()) {
res[(*it1).first] = (*it1).second + (*it2).second;
}
else res[(*it1).first] = (*it1).second;
++it1;
}
}
else {
while (it2 != productesCompra2.end()) {
it1 = productesCompra1.find((*it2).first);
if (it1 != productesCompra1.end()) {
res[(*it2).first] = (*it1).second + (*it2).second;
}
else res[(*it2).first] = (*it2).second;
++it2;
}
}

  return res;
}

quantProductes andOperacioRes(AST *a, quantProductes& res) {
map<string, quantProductes>::iterator it = llistaCompra.find(a->text);
quantProductes productesCompra1 = (*it).second;

  quantProductes::iterator it1 = productesCompra1.begin();
quantProductes::iterator it2 = res.begin();

  if (productesCompra1.size() > res.size()) {
while (it1 != productesCompra1.end()) {
it2 = res.find((*it1).first);
if (it2 != res.end()) {
res[(*it1).first] = (*it1).second + (*it2).second;
}
else res[(*it1).first] = (*it1).second;
++it1;
}
}
else {
while (it2 != res.end()) {
it1 = productesCompra1.find((*it2).first);
if (it1 != productesCompra1.end()) {
res[(*it2).first] = (*it1).second + (*it2).second;
}
++it2;
}
}
res.insert(productesCompra1.begin(), productesCompra1.end());
return res;
}

quantProductes multOperacio(AST *a, AST *b) {
quantProductes res;
map<string, quantProductes>::iterator it = llistaCompra.find(b->text);
quantProductes productesCompra1 = (*it).second;
int val =  atoi(a->kind.c_str());
quantProductes::iterator it1 = productesCompra1.begin();
while (it1 != productesCompra1.end()) {
res[(*it1).first] = (*it1).second * val;
++it1;
}
return res;
}

quantProductes multOperacioRes(AST *a, quantProductes& res) {
int val =  atoi(a->kind.c_str());
quantProductes::iterator it1 = res.begin();
while (it1 != res.end()) {
res[(*it1).first] = (*it1).second * val;
++it1;
}
return res;
}

void operacio(AST *a, quantProductes& res) {
if (a->right == NULL and a->down == NULL) return;
else if (a->right != NULL and a->down != NULL) {
operacio(child(a, 0), res);
}
else if (a->right == NULL and a->down != NULL) {
operacio(child(a, 0), res);
}
else if (a->right != NULL and a->down == NULL) {
operacio(a->right, res);
}
if (a->kind == "*") {
if (res.size() > 0) res = multOperacioRes(child(a, 0), res);
else res = multOperacio(child(a,0), child(a,1));
}
else if (a->kind == "MINUS") {
if (res.size() > 0) {
if (a->down->kind == "id") res = andOperacioRes(child(a, 0), res);
else res = minusOperacioRes(child(a, 1), res);
}
else res = minusOperacio(child(a,0), child(a,1));
}
else if (a->kind == "AND") {
if (res.size() > 0) {
if (a->down->kind == "id") res = andOperacioRes(child(a, 0), res);
else res = andOperacioRes(child(a, 1), res);
}
else res = andOperacio(child(a,0), child(a,1));
}
}

void evaluar(AST *a) {
//Busco a->text que es el co= al map, si existeix, borro el submap, sino recorro el abre afeigint
if (a->right != NULL) {
if (a->right->kind == "[") {
if (llistaCompra.find(a->text) != llistaCompra.end()) {
llistaCompra.erase(llistaCompra.find(a->text));
}
quantProductes productesCompra;
assignacio(child(a->right, 0), productesCompra);
llistaCompra[a->text] = productesCompra;
}
else {
quantProductes res;
operacio(a->right, res);
llistaCompra[a->text] = res;
}
}
else evaluar(child(a,0));
}

void mirarArbre(AST *a) {
if (a == NULL) return;
else if (a->kind == "PRODUCTES") productes(child(a, 0), true);
else if (a->kind == "UNITATS") unitats(child(a, 0), true);
else if (a->kind == "DESVIACIO") stdev(child(a, 0));
else if (a->kind == "=") evaluar(child(a,0));
mirarArbre(a->right);
}

//Fi del meu codi
//-------------------------------------------------------------------------------------------

int main() {
root = NULL;
ANTLR(compres(&root), stdin);
ASTPrint(root);
mirarArbre(root->down);
// cout << "------------------------"<<endl;
// map<string, quantProductes>::iterator i = llistaCompra.begin();
// for (; i != llistaCompra.end(); ++i) {
//   quantProductes productesCompra;
//   productesCompra = (*i).second;
//   cout << (*i).first << endl;
//   quantProductes::iterator it = productesCompra.begin();
//   for (; it != productesCompra.end(); ++it) {
//     cout << (*it).first << " " << (*it).second << endl;
//   }
//   cout << "------------------------"<<endl;
// }
}

void
#ifdef __USE_PROTOS
compres(AST**_root)
#else
compres(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd1[LA(1)]&0x1) ) {
      opers(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  (*_root)=createASTlist(_sibling);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x2);
  }
}

void
#ifdef __USE_PROTOS
opers(AST**_root)
#else
opers(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==ID) ) {
    zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    zzmatch(ASSIG); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    aval(zzSTR); zzlink(_root, &_sibling, &_tail);
  }
  else {
    if ( (setwd1[LA(1)]&0x4) ) {
      resultat(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {zzFAIL(1,zzerr1,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x8);
  }
}

void
#ifdef __USE_PROTOS
aval(AST**_root)
#else
aval(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==CLAUO) ) {
    definicio(zzSTR); zzlink(_root, &_sibling, &_tail);
  }
  else {
    if ( (setwd1[LA(1)]&0x10) ) {
      operacio(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {zzFAIL(1,zzerr2,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x20);
  }
}

void
#ifdef __USE_PROTOS
operacio(AST**_root)
#else
operacio(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  operacio2(zzSTR); zzlink(_root, &_sibling, &_tail);
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd1[LA(1)]&0x40) ) {
      {
        zzBLOCK(zztasp3);
        zzMake0;
        {
        if ( (LA(1)==AND) ) {
          zzmatch(AND); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {
          if ( (LA(1)==MINUS) ) {
            zzmatch(MINUS); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
          }
          else {zzFAIL(1,zzerr3,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
        zzEXIT(zztasp3);
        }
      }
      operacio2(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x80);
  }
}

void
#ifdef __USE_PROTOS
operacio2(AST**_root)
#else
operacio2(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==NUM) ) {
    zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    zzmatch(MULT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    {
      zzBLOCK(zztasp2);
      zzMake0;
      {
      if ( (LA(1)==ID) ) {
        zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      }
      else {
        if ( (LA(1)==PARO) ) {
          zzmatch(PARO);  zzCONSUME;
          operacio(zzSTR); zzlink(_root, &_sibling, &_tail);
          zzmatch(PART);  zzCONSUME;
        }
        else {zzFAIL(1,zzerr4,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
      zzEXIT(zztasp2);
      }
    }
  }
  else {
    if ( (LA(1)==ID) ) {
      zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (LA(1)==PARO) ) {
        zzmatch(PARO);  zzCONSUME;
        operacio(zzSTR); zzlink(_root, &_sibling, &_tail);
        zzmatch(PART);  zzCONSUME;
      }
      else {zzFAIL(1,zzerr5,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x1);
  }
}

void
#ifdef __USE_PROTOS
definicio(AST**_root)
#else
definicio(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    zzmatch(CLAUO); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    llista2(zzSTR); zzlink(_root, &_sibling, &_tail);
    {
      zzBLOCK(zztasp3);
      zzMake0;
      {
      while ( (LA(1)==COMA) ) {
        zzmatch(COMA);  zzCONSUME;
        llista2(zzSTR); zzlink(_root, &_sibling, &_tail);
        zzLOOP(zztasp3);
      }
      zzEXIT(zztasp3);
      }
    }
    zzmatch(CLAUT);  zzCONSUME;
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x2);
  }
}

void
#ifdef __USE_PROTOS
llista2(AST**_root)
#else
llista2(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(PARO);  zzCONSUME;
  zzmatch(NUM); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(COMA);  zzCONSUME;
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(PART);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x4);
  }
}

void
#ifdef __USE_PROTOS
resultat(AST**_root)
#else
resultat(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==UNITATS) ) {
      zzmatch(UNITATS); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (LA(1)==DESVIACIO) ) {
        zzmatch(DESVIACIO); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      }
      else {
        if ( (LA(1)==PRODUCTES) ) {
          zzmatch(PRODUCTES); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {zzFAIL(1,zzerr6,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
    }
    zzEXIT(zztasp2);
    }
  }
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x8);
  }
}
