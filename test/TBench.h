#ifndef ROOT_TBENCH
#define ROOT_TBENCH
   
#include "TClonesArray.h"

#include <vector>
#ifndef R__GLOBALSTL
using std::vector;
#endif
      
//-------------------------------------------------------------
class THit {
protected:
  float     fX;         //x position at center
  float     fY;         //y position at center
  float     fZ;         //z position at center
  int       fNpulses;   //Number of pulses
  int      *fPulses;    //[fNpulses]
  int       fTime[10];  //time at the 10 layers
public:

  THit();
  THit(const THit &);
  THit(int time);
  virtual ~THit();

  void  Set (int time);
  inline int Get(int i) { return fTime[i]; }
  
  friend TBuffer &operator<<(TBuffer &b, const THit *hit);

  ClassDef(THit,1) // the hit class
};

#if defined R__TEMPLATE_OVERLOAD_BUG
template <> 
#endif
inline TBuffer &operator>>(TBuffer &buf,THit *&obj)
{
   obj = new THit();
   obj->Streamer(buf);
   return buf;
}

//-------------------------------------------------------------
class TObjHit : public TObject, public THit {

public:

  TObjHit();
  TObjHit(int time);
  virtual ~TObjHit(){;}

  ClassDef(TObjHit,1) // the hit class
};

//-------------------------------------------------------------
class TSTLhit {
protected:
  Int_t            fNhits;
  vector <THit>    fList1; //||

public:

  TSTLhit();
  TSTLhit(int nmax);
  void Clear(Option_t *option="");
  virtual ~TSTLhit();
  void MakeEvent(int ievent);
  Int_t MakeTree(int mode, int nevents, int compression, int split, float &cx);
  Int_t ReadTree();

  ClassDef(TSTLhit,1) // STL vector of THit
};
      
//-------------------------------------------------------------
class TSTLhitStar {
protected:
  Int_t            fNhits;
  vector <THit*>   fList2; //

public:

  TSTLhitStar();
  TSTLhitStar(int nmax);
  void Clear(Option_t *option="");
  virtual ~TSTLhitStar();
  void MakeEvent(int ievent);
  Int_t MakeTree(int mode, int nevents, int compression, int split, float &cx);
  Int_t ReadTree();

  ClassDef(TSTLhitStar,1) // STL vector of pointers to THit
};
      
//-------------------------------------------------------------
class TCloneshit {
protected:
  Int_t            fNhits;
  TClonesArray    *fList3; //->

public:

  TCloneshit();
  TCloneshit(int nmax);
  void Clear(Option_t *option="");
  virtual ~TCloneshit();
  void MakeEvent(int ievent);
  Int_t MakeTree(int mode, int nevents, int compression, int split, float &cx);
  Int_t ReadTree();

  ClassDef(TCloneshit,1) // TClonesArray of TObjHit
};

#endif
