%!PS-Adobe-3.0 EPSF-3.0
%%Creator: Mayura Draw, Version 4.0
%%Title: SphericalElements.md
%%CreationDate: Wed Nov 17 10:01:26 2004
%%BoundingBox: 152 211 537 618
%%Orientation: Portrait
%%EndComments
%%BeginProlog
%%BeginResource: procset MayuraDraw_ops
%%Version: 4.0
%%Copyright: (c) 1993-2001 Mayura Software
/PDXDict 100 dict def
PDXDict begin
% width height matrix proc key cache
% definepattern -\> font
/definepattern { %def
  7 dict begin
    /FontDict 9 dict def
    FontDict begin
      /cache exch def
      /key exch def
      /proc exch cvx def
      /mtx exch matrix invertmatrix def
      /height exch def
      /width exch def
      /ctm matrix currentmatrix def
      /ptm matrix identmatrix def
      /str
      (xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx)
      def
    end
    /FontBBox [ %def
      0 0 FontDict /width get
      FontDict /height get
    ] def
    /FontMatrix FontDict /mtx get def
    /Encoding StandardEncoding def
    /FontType 3 def
    /BuildChar { %def
      pop begin
      FontDict begin
        width 0 cache { %ifelse
          0 0 width height setcachedevice
        }{ %else
          setcharwidth
        } ifelse
        0 0 moveto width 0 lineto
        width height lineto 0 height lineto
        closepath clip newpath
        gsave proc grestore
      end end
    } def
    FontDict /key get currentdict definefont
  end
} bind def

% dict patternpath -
% dict matrix patternpath -
/patternpath { %def
  dup type /dicttype eq { %ifelse
    begin FontDict /ctm get setmatrix
  }{ %else
    exch begin FontDict /ctm get setmatrix
    concat
  } ifelse
  currentdict setfont
  FontDict begin
    FontMatrix concat
    width 0 dtransform
    round width div exch round width div exch
    0 height dtransform
    round height div exch
    round height div exch
    0 0 transform round exch round exch
    ptm astore setmatrix

    pathbbox
    height div ceiling height mul 4 1 roll
    width div ceiling width mul 4 1 roll
    height div floor height mul 4 1 roll
    width div floor width mul 4 1 roll

    2 index sub height div ceiling cvi exch
    3 index sub width div ceiling cvi exch
    4 2 roll moveto

    FontMatrix ptm invertmatrix pop
    { %repeat
      gsave
        ptm concat
        dup str length idiv { %repeat
          str show
        } repeat
        dup str length mod str exch
        0 exch getinterval show
      grestore
      0 height rmoveto
    } repeat
    pop
  end end
} bind def

% dict patternfill -
% dict matrix patternfill -
/patternfill { %def
  gsave
    eoclip patternpath
  grestore
  newpath
} bind def

/img { %def
  gsave
  /imgh exch def
  /imgw exch def
  concat
  imgw imgh 8
  [imgw 0 0 imgh neg 0 imgh]
  /colorstr 768 string def
  /colorimage where {
    pop
    { currentfile colorstr readhexstring pop }
    false 3 colorimage
  }{
    /graystr 256 string def
    {
      currentfile colorstr readhexstring pop
      length 3 idiv
      dup 1 sub 0 1 3 -1 roll
      {
        graystr exch
        colorstr 1 index 3 mul get 30 mul
        colorstr 2 index 3 mul 1 add get 59 mul
        colorstr 3 index 3 mul 2 add get 11 mul
        add add 100 idiv
        put
      } for
      graystr 0 3 -1 roll getinterval
    } image
  } ifelse
  grestore
} bind def

/arrowhead {
  gsave
    [] 0 setdash
    strokeC strokeM strokeY strokeK setcmykcolor
    2 copy moveto
    4 2 roll exch 4 -1 roll exch
    sub 3 1 roll sub
    exch atan rotate dup scale
    arrowtype
    dup 0 eq {
      -1 2 rlineto 7 -2 rlineto -7 -2 rlineto
      closepath fill
    } if
    dup 1 eq {
      0 3 rlineto 9 -3 rlineto -9 -3 rlineto
      closepath fill
    } if
    dup 2 eq {
      -6 -6 rmoveto 6 6 rlineto -6 6 rlineto
      -1.4142 -1.4142 rlineto 4.5858 -4.5858 rlineto
      -4.5858 -4.5858 rlineto closepath fill
    } if
    dup 3 eq {
      -6 0 rmoveto -1 2 rlineto 7 -2 rlineto -7 -2 rlineto
      closepath fill
    } if
    dup 4 eq {
      -9 0 rmoveto 0 3 rlineto 9 -3 rlineto -9 -3 rlineto
      closepath fill
    } if
    dup 5 eq {
      currentpoint newpath 3 0 360 arc
      closepath fill
    } if
    dup 6 eq {
      2.5 2.5 rmoveto 0 -5 rlineto -5 0 rlineto 0 5 rlineto
      closepath fill
    } if
    pop
  grestore
} bind def

/setcmykcolor where { %ifelse
  pop
}{ %else
  /setcmykcolor {
     /black exch def /yellow exch def
     /magenta exch def /cyan exch def
     cyan black add dup 1 gt { pop 1 } if 1 exch sub
     magenta black add dup 1 gt { pop 1 } if 1 exch sub
     yellow black add dup 1 gt { pop 1 } if 1 exch sub
     setrgbcolor
  } bind def
} ifelse

/RE { %def
  findfont begin
  currentdict dup length dict begin
    { %forall
      1 index /FID ne { def } { pop pop } ifelse
    } forall
    /FontName exch def dup length 0 ne { %if
      /Encoding Encoding 256 array copy def
      0 exch { %forall
        dup type /nametype eq { %ifelse
          Encoding 2 index 2 index put
          pop 1 add
        }{ %else
          exch pop
        } ifelse
      } forall
    } if pop
  currentdict dup end end
  /FontName get exch definefont pop
} bind def

/spacecount { %def
  0 exch
  ( ) { %loop
    search { %ifelse
      pop 3 -1 roll 1 add 3 1 roll
    }{ pop exit } ifelse
  } loop
} bind def

/WinAnsiEncoding [
  39/quotesingle 96/grave 130/quotesinglbase/florin/quotedblbase
  /ellipsis/dagger/daggerdbl/circumflex/perthousand
  /Scaron/guilsinglleft/OE 145/quoteleft/quoteright
  /quotedblleft/quotedblright/bullet/endash/emdash
  /tilde/trademark/scaron/guilsinglright/oe/dotlessi
  159/Ydieresis 164/currency 166/brokenbar 168/dieresis/copyright
  /ordfeminine 172/logicalnot 174/registered/macron/ring
  177/plusminus/twosuperior/threesuperior/acute/mu
  183/periodcentered/cedilla/onesuperior/ordmasculine
  188/onequarter/onehalf/threequarters 192/Agrave/Aacute
  /Acircumflex/Atilde/Adieresis/Aring/AE/Ccedilla
  /Egrave/Eacute/Ecircumflex/Edieresis/Igrave/Iacute
  /Icircumflex/Idieresis/Eth/Ntilde/Ograve/Oacute
  /Ocircumflex/Otilde/Odieresis/multiply/Oslash
  /Ugrave/Uacute/Ucircumflex/Udieresis/Yacute/Thorn
  /germandbls/agrave/aacute/acircumflex/atilde/adieresis
  /aring/ae/ccedilla/egrave/eacute/ecircumflex
  /edieresis/igrave/iacute/icircumflex/idieresis
  /eth/ntilde/ograve/oacute/ocircumflex/otilde
  /odieresis/divide/oslash/ugrave/uacute/ucircumflex
  /udieresis/yacute/thorn/ydieresis
] def

/SymbolEncoding [
  32/space/exclam/universal/numbersign/existential/percent
  /ampersand/suchthat/parenleft/parenright/asteriskmath/plus
  /comma/minus/period/slash/zero/one/two/three/four/five/six
  /seven/eight/nine/colon/semicolon/less/equal/greater/question
  /congruent/Alpha/Beta/Chi/Delta/Epsilon/Phi/Gamma/Eta/Iota
  /theta1/Kappa/Lambda/Mu/Nu/Omicron/Pi/Theta/Rho/Sigma/Tau
  /Upsilon/sigma1/Omega/Xi/Psi/Zeta/bracketleft/therefore
  /bracketright/perpendicular/underscore/radicalex/alpha
  /beta/chi/delta/epsilon/phi/gamma/eta/iota/phi1/kappa/lambda
  /mu/nu/omicron/pi/theta/rho/sigma/tau/upsilon/omega1/omega
  /xi/psi/zeta/braceleft/bar/braceright/similar
  161/Upsilon1/minute/lessequal/fraction/infinity/florin/club
  /diamond/heart/spade/arrowboth/arrowleft/arrowup/arrowright
  /arrowdown/degree/plusminus/second/greaterequal/multiply
  /proportional/partialdiff/bullet/divide/notequal/equivalence
  /approxequal/ellipsis/arrowvertex/arrowhorizex/carriagereturn
  /aleph/Ifraktur/Rfraktur/weierstrass/circlemultiply
  /circleplus/emptyset/intersection/union/propersuperset
  /reflexsuperset/notsubset/propersubset/reflexsubset/element
  /notelement/angle/gradient/registerserif/copyrightserif
  /trademarkserif/product/radical/dotmath/logicalnot/logicaland
  /logicalor/arrowdblboth/arrowdblleft/arrowdblup/arrowdblright
  /arrowdbldown/lozenge/angleleft/registersans/copyrightsans
  /trademarksans/summation/parenlefttp/parenleftex/parenleftbt
  /bracketlefttp/bracketleftex/bracketleftbt/bracelefttp
  /braceleftmid/braceleftbt/braceex
  241/angleright/integral/integraltp/integralex/integralbt
  /parenrighttp/parenrightex/parenrightbt/bracketrighttp
  /bracketrightex/bracketrightbt/bracerighttp/bracerightmid
  /bracerightbt
] def

/patarray [
/leftdiagonal /rightdiagonal /crossdiagonal /horizontal
/vertical /crosshatch /fishscale /wave /brick
] def
/arrowtype 0 def
/fillC 0 def /fillM 0 def /fillY 0 def /fillK 0 def
/strokeC 0 def /strokeM 0 def /strokeY 0 def /strokeK 1 def
/pattern -1 def
/mat matrix def
/mat2 matrix def
/nesting 0 def
/deferred /N def
/c /curveto load def
/c2 { pop pop c } bind def
/C /curveto load def
/C2 { pop pop C } bind def
/e { gsave concat 0 0 moveto } bind def
/F {
  nesting 0 eq { %ifelse
    pattern -1 eq { %ifelse
      fillC fillM fillY fillK setcmykcolor eofill
    }{ %else
      gsave fillC fillM fillY fillK setcmykcolor eofill grestore
      0 0 0 1 setcmykcolor
      patarray pattern get findfont patternfill
    } ifelse
  }{ %else
    /deferred /F def
  } ifelse
} bind def
/f { closepath F } bind def
/K { /strokeK exch def /strokeY exch def
     /strokeM exch def /strokeC exch def } bind def
/k { /fillK exch def /fillY exch def
     /fillM exch def /fillC exch def } bind def
/L /lineto load def
/L2 { pop pop L } bind def
/m /moveto load def
/m2 { pop pop m } bind def
/n /newpath load def
/N {
  nesting 0 eq { %ifelse
    newpath
  }{ %else
    /deferred /N def
  } ifelse
} def
/S {
  nesting 0 eq { %ifelse
    strokeC strokeM strokeY strokeK setcmykcolor stroke
  }{ %else
    /deferred /S def
  } ifelse
} bind def
/s { closepath S } bind def
/Tx { fillC fillM fillY fillK setcmykcolor show
      0 leading neg translate 0 0 moveto } bind def
/T { grestore } bind def
/TX { pop } bind def
/Ts { pop } bind def
/tal { pop } bind def
/tld { pop } bind def
/tbx { pop exch pop sub /jwidth exch def } def
/tpt { %def
  fillC fillM fillY fillK setcmykcolor
  moveto show
} bind def
/tpj { %def
  fillC fillM fillY fillK setcmykcolor
  moveto
  dup stringwidth pop
  3 -1 roll
  exch sub
  1 index spacecount
  dup 0 eq { %ifelse
    pop pop show
  }{ %else
    div 0 8#040 4 -1 roll widthshow
  } ifelse
} bind def
/u {} def
/U {} def
/*u { /nesting nesting 1 add def } def
/*U {
  /nesting nesting 1 sub def
  nesting 0 eq {
    deferred cvx exec
  } if
} def
/w /setlinewidth load def
/d /setdash load def
/B {
  nesting 0 eq { %ifelse
    gsave F grestore S
  }{ %else
    /deferred /B def
  } ifelse
} bind def
/b { closepath B } bind def
/z { /align exch def pop /leading exch def exch findfont
     exch scalefont setfont } bind def
/tfn { exch findfont
     exch scalefont setfont } bind def
/Pat { /pattern exch def } bind def
/cm { 6 array astore concat } bind def
/q { mat2 currentmatrix pop } bind def
/Q { mat2 setmatrix } bind def
/Ah {
  pop /arrowtype exch def
  currentlinewidth 5 1 roll arrowhead
} bind def
/Arc {
  mat currentmatrix pop
    translate scale 0 0 1 5 -2 roll arc
  mat setmatrix
} bind def
/Arc2 { pop pop Arc } bind def
/Bx {
  mat currentmatrix pop
    concat /y1 exch def /x1 exch def /y2 exch def /x2 exch def
    x1 y1 moveto x1 y2 lineto x2 y2 lineto x2 y1 lineto
  mat setmatrix
} bind def
/Rr {
  mat currentmatrix pop
    concat /yrad exch def /xrad exch def
    2 copy gt { exch } if /x2 exch def /x1 exch def
    2 copy gt { exch } if /y2 exch def /y1 exch def
    x1 xrad add y2 moveto
    matrix currentmatrix x1 xrad add y2 yrad sub translate xrad yrad scale
    0 0 1 90 -180 arc setmatrix
    matrix currentmatrix x1 xrad add y1 yrad add translate xrad yrad scale
    0 0 1 180 270 arc setmatrix
    matrix currentmatrix x2 xrad sub y1 yrad add translate xrad yrad scale
    0 0 1 270 0 arc setmatrix
    matrix currentmatrix x2 xrad sub y2 yrad sub translate xrad yrad scale
    0 0 1 0 90 arc setmatrix
    closepath
  mat setmatrix
} bind def
/Ov {
  mat currentmatrix pop
    concat translate scale 1 0 moveto 0 0 1 0 360 arc closepath
  mat setmatrix
} bind def
end
%%EndResource
%%EndProlog
%%BeginSetup
%PDX g 3 3 0 0
PDXDict begin
%%EndSetup
%%Page: 1 1
%%BeginPageSetup
/_PDX_savepage save def

15 15 [300 72 div 0 0 300 72 div 0 0]
{ %definepattern
  2 setlinecap
  7.5 0 moveto 15 7.5 lineto
  0 7.5 moveto 7.5 15 lineto
  2 setlinewidth stroke
} bind
/rightdiagonal true definepattern pop

15 15 [300 72 div 0 0 300 72 div 0 0]
{ %definepattern
  2 setlinecap
  7.5 0 moveto 0 7.5 lineto
  15 7.5 moveto 7.5 15 lineto
  2 setlinewidth stroke
} bind
/leftdiagonal true definepattern pop

15 15 [300 72 div 0 0 300 72 div 0 0]
{ %definepattern
  2 setlinecap
  0 7.5 moveto 15 7.5 lineto
  2 setlinewidth stroke
} bind
/horizontal true definepattern pop

15 15 [300 72 div 0 0 300 72 div 0 0]
{ %definepattern
  2 setlinecap
  7.5 0 moveto 7.5 15 lineto
  2 setlinewidth stroke
} bind
/vertical true definepattern pop

15 15 [300 72 div 0 0 300 72 div 0 0]
{ %definepattern
  2 setlinecap
  0 7.5 moveto 15 7.5 lineto
  7.5 0 moveto 7.5 15 lineto
  2 setlinewidth stroke
} bind
/crosshatch true definepattern pop

30 30 [300 72 div 0 0 300 72 div 0 0]
{ %definepattern
  2 setlinecap
  0 7.5 moveto 30 7.5 lineto
  0 22.5 moveto 30 22.5 lineto
  7.5 0 moveto 7.5 7.5 lineto
  7.5 22.5 moveto 7.5 30 lineto
  22.5 7.5 moveto 22.5 22.5 lineto
  1 setlinewidth stroke
} bind
/brick true definepattern pop

30 30 [300 72 div 0 0 300 72 div 0 0]
{ %definepattern
  2 2 scale
  2 setlinecap
  7.5 0 moveto 15 7.5 lineto
  0 7.5 moveto 7.5 15 lineto
  7.5 0 moveto 0 7.5 lineto
  15 7.5 moveto 7.5 15 lineto
  0.5 setlinewidth stroke
} bind
/crossdiagonal true definepattern pop

30 30 [300 72 div 0 0 300 72 div 0 0]
{ %definepattern
  2 2 scale
  1 setlinecap
  0 7.5 moveto 0 15 7.5 270 360 arc
  7.5 15 moveto 15 15 7.5 180 270 arc
  0 7.5 moveto 7.5 7.5 7.5 180 360 arc
  0.5 setlinewidth stroke
} bind
/fishscale true definepattern pop

30 30 [300 72 div 0 0 300 72 div 0 0]
{ %definepattern
  1 setlinecap 0.5 setlinewidth
  7.5 0 10.6 135 45 arcn
  22.5 15 10.6 225 315 arc
  stroke
  7.5 15 10.6 135 45 arcn
  22.5 30 10.6 225 315 arc
  stroke
} bind
/wave true definepattern pop

newpath 2 setlinecap 0 setlinejoin 2 setmiterlimit
[] 0 setdash
152 211 moveto 152 618 lineto 537 618 lineto 537 211 lineto closepath clip
newpath
%%EndPageSetup
1 w
286.963 401.759 285.154 404.601 [0.9786 -0.206 0.206 0.9786 -90.68 49.55] Bx
f
0.5 w
23.3016 23.3017 266.353 462.422 [4 0 0 4 -787.7 -1444] Ov
s
1 w
q
1.2 0 0 1.2 -88.2 -329.3 cm
304.181 612.631 m
505.43 612.572 513.43 612.57 L2
Q
S
q
1.2 0 0 1.2 -88.2 -329.3 cm
304.181 612.631 513.43 612.57 4 2 Ah
Q
q
1.2 0 0 1.2 -88.49 -330.4 cm
304.117 613.693 m
304.273 774.548 304.281 782.548 L2
Q
S
q
1.2 0 0 1.2 -88.49 -330.4 cm
304.117 613.693 304.281 782.548 4 2 Ah
Q
q
1.2 0 0 1.2 -88.74 -331 cm
304.117 613.654 m
213.446 467.198 209.235 460.396 L2
Q
S
q
1.2 0 0 1.2 -88.74 -331 cm
304.117 613.654 209.235 460.396 4 2 Ah
Q
q
1 0 0 1 -15.45 -27.8 cm
293.271 434.983 m
460.127 581.744 466.134 587.027 L2
Q
S
q
1 0 0 1 -15.45 -27.8 cm
293.271 434.983 466.134 587.027 4 2 Ah
Q
0.5 w
[3 3] 0 d
q
1 0 0 1 22.66 -66.94 cm
429.061 626.16 m
430.091 370.77 L
Q
S
q
1 0 0 1 -150.8 -182.8 cm
430.091 589.087 m
603.593 487.667 L
Q
S
q
1 0 0 1 -41.71 -285.8 cm
494.454 589.629 m
253.48 588.572 L
Q
S
1 w
q
1.2 0 0 1.2 143.2 -331.9 cm
292.319 613.097 m
258.78 531.464 L
Q
S
1 1 1 0 k
0.5 w
2.9127 2.91272 99.5257 434.617 [1 0 0 1 155.5 -63.84] Ov
f
2.9127 2.91272 99.5257 434.617 [1 0 0 1 234.3 -59.22] Ov
f
u
0 0 0 0 k
1 w
[] 0 d
q
1 0 0 1 -155.9 320.5 cm
339.896 82.958 m
340.81 79.2273 341.279 77.6309 341.774 76.5722 c
342.445 75.136 344.465 72.1102 345.53 70.9376 c
346.559 69.805 349.372 67.6653 350.601 66.8056 c
352.326 65.5992 356.608 63.0527 358.49 62.1102 c
360.406 61.1505 365.143 59.3126 367.129 58.5417 c
368.998 57.8164 373.474 56.1296 375.393 55.5366 c
377.213 54.9744 381.621 53.9079 383.469 53.4706 c
385.43 53.0068 390.122 51.9449 392.109 51.5924 c
394.58 51.1541 400.518 50.4133 403.002 50.0899 c
405.097 49.8172 410.099 49.1503 412.206 48.963 c
414.514 48.7578 420.036 48.51 422.348 48.3995 c
424.486 48.2973 429.601 48.202 431.739 48.0239 c
433.992 47.8361 440.586 48.2881 443.195 48.3995 c
445.636 48.5037 451.464 48.7569 453.901 48.963 c
456.48 49.181 462.61 49.9008 465.17 50.2777 c
468.312 50.7404 475.77 52.0818 478.881 52.7193 c
481.292 53.2137 487.027 54.4773 489.399 55.161 c
491.668 55.8153 496.963 57.6992 499.165 58.5417 c
501.335 59.3719 506.475 61.4261 508.556 62.4858 c
510.261 63.3538 514.135 65.6946 515.693 66.8056 c
516.789 67.5871 519.231 69.6307 520.201 70.562 c
521.08 71.4062 523.062 73.4941 523.769 74.5062 c
524.305 75.2735 525.349 77.1746 525.647 78.0747 c
525.915 78.8811 526.112 80.8248 526.211 81.6432 c
526.257 82.0275 526.304 82.591 526.399 83.897 c
Q
S
[3 3] 0 d
q
0.9999 0.01007 0.01007 -0.9999 -156.7 483.4 cm
339.896 82.958 m
340.81 79.2273 341.279 77.6309 341.774 76.5722 c
342.445 75.136 344.465 72.1102 345.53 70.9376 c
346.559 69.805 349.372 67.6653 350.601 66.8056 c
352.326 65.5992 356.608 63.0527 358.49 62.1102 c
360.406 61.1505 365.143 59.3126 367.129 58.5417 c
368.998 57.8164 373.474 56.1296 375.393 55.5366 c
377.213 54.9744 381.621 53.9079 383.469 53.4706 c
385.43 53.0068 390.122 51.9449 392.109 51.5924 c
394.58 51.1541 400.518 50.4133 403.002 50.0899 c
405.097 49.8172 410.099 49.1503 412.206 48.963 c
414.514 48.7578 420.036 48.51 422.348 48.3995 c
424.486 48.2973 429.601 48.202 431.739 48.0239 c
433.992 47.8361 440.586 48.2881 443.195 48.3995 c
445.636 48.5037 451.464 48.7569 453.901 48.963 c
456.48 49.181 462.61 49.9008 465.17 50.2777 c
468.312 50.7404 475.77 52.0818 478.881 52.7193 c
481.292 53.2137 487.027 54.4773 489.399 55.161 c
491.668 55.8153 496.963 57.6992 499.165 58.5417 c
501.335 59.3719 506.475 61.4261 508.556 62.4858 c
510.261 63.3538 514.135 65.6946 515.693 66.8056 c
516.789 67.5871 519.231 69.6307 520.201 70.562 c
521.08 71.4062 523.062 73.4941 523.769 74.5062 c
524.305 75.2735 525.349 77.1746 525.647 78.0747 c
525.915 78.8811 526.112 80.8248 526.211 81.6432 c
526.257 82.0275 526.304 82.591 526.399 83.897 c
Q
S
U
q
-0.2544 0.9671 -0.9671 -0.2544 732.1 567.4 cm
161.375 -179.463 247.924 247.924 152.045 435.647 159.526 -177.614 Arc2
Q
S
q
-0.2544 0.9671 -0.9671 -0.2544 732.1 567.4 cm
-87.707 498.777 -80.2182 522.366 4 1 Ah
-95.4591 450.075 -95.6643 425.326 4 2 Ah
Q
q
-0.9982 -0.06034 0.06034 -0.9982 301.3 874.1 cm
155.955 -164.084 251.733 251.733 152.045 435.647 154.134 -162.263 Arc2
Q
S
q
-0.9982 -0.06034 0.06034 -0.9982 301.3 874.1 cm
-83.9423 523.282 -74.4686 545.47 4 1 Ah
-93.9629 382.262 -87.7219 358.957 4 2 Ah
Q
0 0 0 0 K
0.5 w
[] 0 d
450.687 474.779 433.18 490.226 [1 0 0 1 -15.45 -27.8] Bx
b
450.687 474.779 433.18 490.226 [1 0 0 1 -142.1 -127.7] Bx
b
%%PageTrailer
_PDX_savepage restore
%%Trailer
end
showpage
%%EOF
