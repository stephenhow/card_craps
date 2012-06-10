//
//  craps.h
//  cardcraps
//
//  Created by Stephen How on 6/2/12.
//  Copyright (c) 2012 DiscountGambling.net. All rights reserved.
//

#ifndef CRAPS_H
#define CRAPS_H

#include <vector>

using namespace std;

class Roll {
protected:
    int die1, die2;
public:
    Roll();
    void setDie(int d1, int d2);
    bool isCraps();
    bool isBoxcars();
    bool isSeven();
    bool isYo();
    int getValue();
    void copyFrom(Roll&);
    int countValue(int);
    int getDie1();
    int getDie2();
	bool isValid();
};

class Dice {
public:
    Dice();
    virtual void getRoll(Roll&);
    virtual void muckRoll(Roll&);
	virtual bool countdown();
};

class CSMDice : public Dice {
protected:
    vector<int> buffer;
    vector<int> reservoir;
    int minBufferDepth;
	int sets;
public:
    CSMDice(int sets, int minDepth);
    virtual void getRoll(Roll&);
    virtual void muckRoll(Roll&);
	virtual bool countdown();
};

#define NUM_SLOTS 40
#define MAX_CARDS_PER_SLOT 14

class CSM126Dice : public CSMDice {
protected:
    vector<int> slots[NUM_SLOTS];
    void dropSlot();
    void muckCard(int);
public:
    CSM126Dice(int sets, int minDepth);
    virtual void getRoll(Roll&);
    virtual void muckRoll(Roll&);
	virtual bool countdown();
};

class Averager {
protected:
    unsigned long samples;
    double total;
public:
    Averager();
    void add(double value);
    double getValue();
};

#endif
