/* @(#)root/base:$Name:  $:$Id: LinkDef3.h,v 1.8 2002/12/04 12:13:32 rdm Exp $ */

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ global kDefaultScrollBarWidth;
#pragma link C++ global kNone;
#pragma link C++ global kCopyFromParent;
#pragma link C++ global kParentRelative;
#pragma link C++ global kWABackPixmap;
#pragma link C++ global kWABackPixel;
#pragma link C++ global kWABorderPixmap;
#pragma link C++ global kWABorderPixel;
#pragma link C++ global kWABorderWidth;
#pragma link C++ global kWABitGravity;
#pragma link C++ global kWAWinGravity;
#pragma link C++ global kWABackingStore;
#pragma link C++ global kWABackingPlanes;
#pragma link C++ global kWABackingPixel;
#pragma link C++ global kWAOverrideRedirect;
#pragma link C++ global kWASaveUnder;
#pragma link C++ global kWAEventMask;
#pragma link C++ global kWADontPropagate;
#pragma link C++ global kWAColormap;
#pragma link C++ global kWACursor;

#pragma link C++ global kNoEventMask;
#pragma link C++ global kKeyPressMask;
#pragma link C++ global kKeyReleaseMask;
#pragma link C++ global kButtonPressMask;
#pragma link C++ global kButtonReleaseMask;
#pragma link C++ global kPointerMotionMask;
#pragma link C++ global kButtonMotionMask;
#pragma link C++ global kExposureMask;
#pragma link C++ global kStructureNotifyMask;
#pragma link C++ global kEnterWindowMask;
#pragma link C++ global kLeaveWindowMask;
#pragma link C++ global kFocusChangeMask;
#pragma link C++ global kOwnerGrabButtonMask;
#pragma link C++ global kColormapChangeMask;

#pragma link C++ global kKeyShiftMask;
#pragma link C++ global kKeyLockMask;
#pragma link C++ global kKeyControlMask;
#pragma link C++ global kKeyMod1Mask;
#pragma link C++ global kButton1Mask;
#pragma link C++ global kButton2Mask;
#pragma link C++ global kButton3Mask;
#pragma link C++ global kButton4Mask;
#pragma link C++ global kButton5Mask;
#pragma link C++ global kAnyModifier;

#pragma link C++ global kGCFunction;
#pragma link C++ global kGCPlaneMask;
#pragma link C++ global kGCForeground;
#pragma link C++ global kGCBackground;
#pragma link C++ global kGCLineWidth;
#pragma link C++ global kGCLineStyle;
#pragma link C++ global kGCCapStyle;
#pragma link C++ global kGCJoinStyle;
#pragma link C++ global kGCFillStyle;
#pragma link C++ global kGCFillRule;
#pragma link C++ global kGCTile;
#pragma link C++ global kGCStipple;
#pragma link C++ global kGCTileStipXOrigin;
#pragma link C++ global kGCTileStipYOrigin;
#pragma link C++ global kGCFont;
#pragma link C++ global kGCSubwindowMode;
#pragma link C++ global kGCGraphicsExposures;
#pragma link C++ global kGCClipXOrigin;
#pragma link C++ global kGCClipYOrigin;
#pragma link C++ global kGCClipMask;
#pragma link C++ global kGCDashOffset;
#pragma link C++ global kGCDashList;
#pragma link C++ global kGCArcMode;

#pragma link C++ global kDoRed;
#pragma link C++ global kDoGreen;
#pragma link C++ global kDoBlue;

#pragma link C++ global kPAColormap;
#pragma link C++ global kPADepth;
#pragma link C++ global kPASize;
#pragma link C++ global kPAHotspot;
#pragma link C++ global kPAReturnPixels;
#pragma link C++ global kPACloseness;

#pragma link C++ global kPrimarySelection;
#pragma link C++ global kCutBuffer;
#pragma link C++ global kMaxPixel;

#pragma link C++ enum EGuiConstants;
#pragma link C++ enum EGEventType;
#pragma link C++ enum EGraphicsFunction;
#pragma link C++ enum EGraphicsFunction;
#pragma link C++ enum EMouseButton;
#pragma link C++ enum EXMagic;
#pragma link C++ enum EInitialState;
#pragma link C++ enum EKeySym;
#pragma link C++ enum EEventType;

#pragma link C++ typedef timespec_t;
#pragma link C++ typedef Handle_t;
#pragma link C++ typedef Display_t;
#pragma link C++ typedef Visual_t;
#pragma link C++ typedef Window_t;
#pragma link C++ typedef Pixmap_t;
#pragma link C++ typedef Drawable_t;
#pragma link C++ typedef Region_t;
#pragma link C++ typedef Colormap_t;
#pragma link C++ typedef Cursor_t;
#pragma link C++ typedef FontH_t;
#pragma link C++ typedef KeySym_t;
#pragma link C++ typedef Atom_t;
#pragma link C++ typedef GContext_t;
#pragma link C++ typedef FontStruct_t;
#pragma link C++ typedef Mask_t;
#pragma link C++ typedef Time_t;

#pragma link C++ struct Event_t;
#pragma link C++ struct SetWindowAttributes_t;
#pragma link C++ struct WindowAttributes_t;
#pragma link C++ struct GCValues_t;
#pragma link C++ struct ColorStruct_t;
#pragma link C++ struct PictureAttributes_t;
#pragma link C++ struct Segment_t;
#pragma link C++ struct Point_t;
#pragma link C++ struct Rectangle_t;
#pragma link C++ struct timespec;

#pragma link C++ function operator<<(ostream&, const TTimeStamp&);
#pragma link C++ function operator<<(TBuffer&, const TTimeStamp&);
#pragma link C++ function operator>>(TBuffer&, TTimeStamp&);
#pragma link C++ function operator==(const TTimeStamp&, const TTimeStamp&);
#pragma link C++ function operator!=(const TTimeStamp&, const TTimeStamp&);
#pragma link C++ function operator< (const TTimeStamp&, const TTimeStamp&);
#pragma link C++ function operator<=(const TTimeStamp&, const TTimeStamp&);
#pragma link C++ function operator> (const TTimeStamp&, const TTimeStamp&);
#pragma link C++ function operator>=(const TTimeStamp&, const TTimeStamp&);

#pragma link C++ class TTimeStamp+;
#pragma link C++ class TVirtualMutex;
#pragma link C++ class TLockGuard;
#pragma link C++ class TVirtualProof;

#pragma link C++ typedef TObjNumC;
#pragma link C++ typedef TObjNumUC;
#pragma link C++ typedef TObjNumS;
#pragma link C++ typedef TObjNumUS;
#pragma link C++ typedef TObjNumI;
#pragma link C++ typedef TObjNumUI;
#pragma link C++ typedef TObjNumL;
#pragma link C++ typedef TObjNumUL;
#pragma link C++ typedef TObjNumF;
#pragma link C++ typedef TObjNumD;
#pragma link C++ typedef TObjPtr;

#pragma link C++ class TObjNum<Char_t>+;
#pragma link C++ class TObjNum<UChar_t>+;
#pragma link C++ class TObjNum<Short_t>+;
#pragma link C++ class TObjNum<UShort_t>+;
#pragma link C++ class TObjNum<Int_t>+;
#pragma link C++ class TObjNum<UInt_t>+;
#pragma link C++ class TObjNum<Long_t>+;
#pragma link C++ class TObjNum<ULong_t>+;
#pragma link C++ class TObjNum<Float_t>+;
#pragma link C++ class TObjNum<Double_t>+;
#pragma link C++ class TObjNum<void*>+;

#endif
