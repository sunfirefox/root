#ifndef ROOT_TGeoGedFrame
#define ROOT_TGeoGedFrame

#ifndef ROOT_TGedFrame
#include "TGedFrame.h"
#endif

class TGTab;
class TGeoTabManager;
class TVirtualPad;

class TGeoGedFrame : public TGedFrame
{
protected:
   TGTab          *fTab;
   TGeoTabManager *fTabMgr;
   TVirtualPad    *fPad;           //selected pad, if exists

public:
   TGeoGedFrame(const TGWindow *p = 0,
                Int_t width = 140, Int_t height = 30,
                UInt_t options = kChildFrame,
                Pixel_t back = GetDefaultFrameBackground());

   virtual void SetGedEditor(TGedEditor* ed);
   virtual void SetActive(Bool_t active = kTRUE);
   virtual void Update();

   ClassDef(TGeoGedFrame, 0)
};

#endif
