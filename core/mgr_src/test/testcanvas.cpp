//! \file testcanvas->cpp
//! \brief Implement the testing class: Testcanvas->
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#include "testcanvas.h"
#include "gicanvas.h"
#include <stdlib.h>

static bool s_inited = false;
static bool s_randStyle = false;

void TestCanvas::initRand()
{
    srand(9999);
    s_inited = true;
}

int TestCanvas::randInt(int minv, int maxv)
{
    return rand() % (maxv - minv + 1) + minv;
}

float TestCanvas::randFloat(float minv, float maxv)
{
    float div = 10.f;
    int range = (int)((maxv - minv) * div + 0.5f);
    return (float)(rand() % range) / div + minv;
}

void TestCanvas::test(GiCanvas* canvas, int bits, int n, bool randStyle)
{
    s_randStyle = randStyle;
    if ((bits & 0x400) == 0 || !s_inited) {
        initRand();
    }
    
    if (bits & 0x01)
        testRect(canvas, n);
    if (bits & 0x02)
        testLine(canvas, n);
    if (bits & 0x04)
        testTextAt(canvas, n);
    if (bits & 0x08)
        testEllipse(canvas, n);
    if (bits & 0x10)
        testQuadBezier(canvas, n);
    if (bits & 0x20)
        testCubicBezier(canvas, n);
    if (bits & 0x40)
        testPolygon(canvas, n);
    if (bits & 0x80)
        canvas->clearRect(100, 100, 200, 200);
    if (bits & 0x100)
        testClipPath(canvas, n);
    if (bits & 0x200)
        testHandle(canvas, n);
    if (bits & 0x400)
        testDynCurves(canvas);
}

void TestCanvas::testRect(GiCanvas* canvas, int n)
{
    for (int i = 0; i < n; i++) {
        if (s_randStyle) {
            canvas->setPen(randInt(10, 0xFF) << 24 | randInt(0, 0xFFFFFF), -1.f, -1, 0);
            canvas->setBrush(randInt(10, 0xFF) << 24 | randInt(0, 0xFFFFFF), 0);
        }
        canvas->drawRect(randFloat(10.f, 600.f), randFloat(10.f, 600.f),
                        randFloat(10.f, 400.f), randFloat(10.f, 400.f),
                        randInt(0, 1) == 1, randInt(0, 1) == 1);
    }
}

void TestCanvas::testLine(GiCanvas* canvas, int n)
{
    for (int i = 0; i < n; i++) {
        if (s_randStyle) {
            canvas->setPen(randInt(10, 0xFF) << 24 | randInt(0, 0xFFFFFF), -1.f, -1, 0);
        }
        canvas->drawLine(randFloat(10.f, 600.f), randFloat(10.f, 600.f),
                        randFloat(10.f, 400.f), randFloat(10.f, 400.f));
    }
}

void TestCanvas::testEllipse(GiCanvas* canvas, int n)
{
    for (int i = 0; i < n; i++) {
        if (s_randStyle) {
            canvas->setPen(randInt(10, 0xFF) << 24 | randInt(0, 0xFFFFFF), -1.f, -1, 0);
            canvas->setBrush(randInt(10, 0xFF) << 24 | randInt(0, 0xFFFFFF), 0);
        }
        canvas->drawEllipse(randFloat(10.f, 600.f), randFloat(10.f, 600.f),
                           randFloat(10.f, 400.f), randFloat(10.f, 400.f),
                           randInt(0, 1) == 1, randInt(0, 1) == 1);
    }
}

void TestCanvas::testQuadBezier(GiCanvas* canvas, int n)
{
    float x1 = randFloat(100.f, 400.f);
    float y1 = randFloat(100.f, 400.f);
    
    for (int i = 0; i < n; i++) {
        canvas->beginPath();
        
        float x2 = x1 + randFloat(-100.f, 100.f);
        float y2 = y1 + randFloat(-100.f, 100.f);
        float x3 = x2 + randFloat(-100.f, 100.f);
        float y3 = y2 + randFloat(-100.f, 100.f);
        
        canvas->moveTo(x1, y1);
        canvas->lineTo((x1 + x2) / 2, (y1 + y2) / 2);
        
        for (int j = randInt(5, 20); j > 0; j--) {
            canvas->quadTo(x2, y2, (x3 + x2) / 2, (y3 + y2) / 2);
            
            x1 = x2; x2 = x3;
            y1 = y2; y2 = y3;
            x3 = x2 + randFloat(-100.f, 100.f);
            y3 = y2 + randFloat(-100.f, 100.f);
        }
        canvas->lineTo(x2, y2);
        
        if (s_randStyle) {
            canvas->setPen(0xFF000000 | randInt(0, 0xFFFFFF), randFloat(0, 6), randInt(0, 4), 0);
        }
        canvas->drawPath(true, false);
    }
}

void TestCanvas::testCubicBezier(GiCanvas* canvas, int n)
{
    float x1 = randFloat(100.f, 400.f);
    float y1 = randFloat(100.f, 400.f);
    
    for (int i = 0; i < n; i++) {
        canvas->beginPath();
        
        float x2 = x1 + randFloat(-50.f, 50.f);
        float y2 = y1 + randFloat(-50.f, 50.f);
        float x3 = x2 + randFloat(-50.f, 50.f);
        float y3 = y2 + randFloat(-50.f, 50.f);
        float x4 = x3 + randFloat(-50.f, 50.f);
        float y4 = y3 + randFloat(-50.f, 50.f);
        
        canvas->moveTo(x1, y1);
        
        for (int j = randInt(1, 10); j > 0; j--) {
            canvas->bezierTo(x2, y2, x3, y3, x4, y4);
            
            x1 = x2; y1 = y2;                   // P2
            x2 = 2 * x4 - x3;                   // Q2=2P4-P3
            y2 = 2 * y4 - y3;
            x3 = 4 * (x4 - x3) + x1;            // Q3=4(P4-P3)+P2
            y3 = 4 * (y4 - y3) + y1;
            x4 = x3 + randFloat(-50.f, 50.f);
            y4 = y3 + randFloat(-50.f, 50.f);
        }
        
        if (s_randStyle) {
            canvas->setPen(0xFF000000 | randInt(0, 0xFFFFFF), -1.f, -1, 0);
        }
        canvas->drawPath(true, false);
    }
}

void TestCanvas::testDynCurves(GiCanvas* canvas)
{
    static float x1, y1, x2, y2, x3, y3, x4, y4;
    static float xy[2 * (3 * 100 + 1)] = { 0 };
    static int n = 0;
    
    if (!s_inited) {
        initRand();
    }
    if (n == 0) {
        x1 = randFloat(100.f, 400.f);
        y1 = randFloat(100.f, 400.f);
        x2 = x1 + randFloat(-20.f, 20.f);
        y2 = y1 + randFloat(-20.f, 20.f);
        x3 = x2 + randFloat(-20.f, 20.f);
        y3 = y2 + randFloat(-20.f, 20.f);
    }
    else if (n == sizeof(xy)/sizeof(xy[0])) {
        for (int i = 0; i + 6 < n; i++) {
            xy[i] = xy[i + 6];
        }
        n -= 6;
    }
    x4 = x3 + randFloat(-20.f, 20.f);
    y4 = y3 + randFloat(-20.f, 20.f);
    if (x4 < 0 || y4 < 0 || x4 > 2048 || y4 > 2048) {
        n = 0;
        return;
    }
    
    canvas->beginPath();
    
    if (n >= 8) {
        canvas->moveTo(xy[0], xy[1]);
        for (int i = 2; i + 5 < n; i += 6) {
            canvas->bezierTo(xy[i], xy[i+1],
                            xy[i+2], xy[i+3], xy[i+4], xy[i+5]);
        }
    }
    else {
        canvas->moveTo(x1, y1);
        xy[0] = x1;
        xy[1] = y1;
        n = 2;
    }
    
    canvas->bezierTo(x2, y2, x3, y3, x4, y4);
    canvas->drawPath(true, false);
    
    xy[n++] = x2;
    xy[n++] = y2;
    xy[n++] = x3;
    xy[n++] = y3;
    xy[n++] = x4;
    xy[n++] = y4;
    
    x1 = x2; y1 = y2;                   // P2
    x2 = 2 * x4 - x3;                   // Q2=2P4-P3
    y2 = 2 * y4 - y3;
    x3 = 4 * (x4 - x3) + x1;            // Q3=4(P4-P3)+P2
    y3 = 4 * (y4 - y3) + y1;
    x1 = x4; y1 = y4;                   // Q1=P4
}

void TestCanvas::testPolygon(GiCanvas* canvas, int n)
{
    for (int i = 0; i < n; i++) {
        canvas->beginPath();
        
        float x = randFloat(10.f, 600.f);
        float y = randFloat(10.f, 600.f);
        canvas->moveTo(x, y);
        
        for (int j = randInt(1, 5); j > 0; j--) {
            canvas->lineTo(x += randFloat(-100.f, 100.f), y += randFloat(-100.f, 100.f));
        }
        canvas->closePath();
        
        if (s_randStyle) {
            canvas->setPen(0x8F000000 | randInt(0, 0xFFFFFF), -1.f, -1, 0);
            canvas->setBrush(0x41000000 | randInt(0, 0xFFFFFF), 0);
        }
        canvas->drawPath(randInt(0, 1) == 1, randInt(0, 1) == 1);
    }
}

void TestCanvas::testClipPath(GiCanvas* canvas, int n)
{
    for (int i = 0; i < 2; i++) {
        canvas->saveClip();
        if (canvas->clipRect(randFloat(100.f, 500.f), randFloat(100.f, 500.f),
                            randFloat(50.f, 200.f), randFloat(50.f, 200.f))) {
            testLine(canvas, n);
        }
        canvas->restoreClip();
    }
    
    canvas->saveClip();
    canvas->beginPath();
    
    for (int j = 0; j < 5; j++) {
        float x1 = randFloat(100.f, 400.f);
        float y1 = randFloat(100.f, 400.f);
        canvas->moveTo(x1, y1);
        
        for (int k = randInt(2, 4); k > 0; k--) {
            float x2 = x1 + randFloat(-150.f, 150.f);
            float y2 = y1 + randFloat(-150.f, 150.f);
            float x3 = x2 + randFloat(-150.f, 150.f);
            float y3 = y2 + randFloat(-150.f, 150.f);
            x1 = x3 + randFloat(-150.f, 150.f);
            y1 = y3 + randFloat(-150.f, 150.f);
            canvas->bezierTo(x2, y2, x3, y3, x1, y1);
        }
        canvas->closePath();
    }
    
    if (canvas->clipPath()) {
        if (s_randStyle) {
            canvas->setPen(0x41000000 | randInt(0, 0xFFFFFF), -1.f, -1, 0);
            canvas->setBrush(0x41000000 | randInt(0, 0xFFFFFF), 0);
        }
        canvas->drawRect(0, 0, 1000, 1000, true, true);
        testCubicBezier(canvas, n);
    }
    canvas->restoreClip();
}

void TestCanvas::testHandle(GiCanvas* canvas, int n)
{
    float w = 80;
    float y = w;
    canvas->drawLine(40,  0, 40,  30 * w);
    canvas->drawLine(200, 0, 200, 30 * w);
    canvas->drawLine(400, 0, 400, 30 * w);
    canvas->drawLine(600, 0, 600, 30 * w);
    
    for (int i = 0; i < n; i++) {
        canvas->drawLine(0, y, 650, y);
        canvas->drawHandle(40, y, i);
        float h = 10 + (float)i * 20;
        canvas->drawBitmap(NULL, 200, y, h, h, 0);
        canvas->drawBitmap(NULL, 400, y, 57, 57,
                          3.1415926f * (float)i / 6);
        canvas->drawBitmap(NULL, 600, y, h, h,
                          3.1415926f * (float)i / 6);
        y += ((h < 57.f) ? 57.f : h) + 5;
    }
}

void TestCanvas::testTextAt(GiCanvas* canvas, int n)
{
    float w, h, y;
    const char* text = "汉fjl123 Abc@汉字";
    canvas->setBrush(0x88000000 | randInt(0, 0xFFFFFF), 0);
    
    canvas->drawLine(0, 50, 1000, 50);
    for (h = 5, y = 50; h < 80; y += h, h += 10) {
        w = canvas->drawTextAt(text, 500, y, h, 2);
        canvas->drawRect(500 - w, y, w, h, true, false);
    }
    canvas->drawLine(0, 400, 1000, 400);
    for (h = 5, y = 400; h < 120; y += h, h += 10) {
        w = canvas->drawTextAt(text, 50, y, h, 0);
        canvas->drawRect(50, y, w, h, true, false);
    }
    text = "f?？abcdefghijklmnop汉字qrstuvwxyz";
    for (; h < (float)n; y += h, h += 10) {
        w = canvas->drawTextAt(text, 50, y, h, 0);
        canvas->drawRect(50, y, w, h, true, false);
    }
}
