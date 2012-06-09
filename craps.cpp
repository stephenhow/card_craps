//
//  craps.cpp
//  cardcraps
//
//  Created by Stephen How on 6/2/12.
//  Copyright (c) 2012 DiscountGambling.net. All rights reserved.
//

#include "craps.h"
#include <iostream>

Roll::Roll() {
    die1 = 0;
    die2 = 0;
}

void Roll::setDie(int d1, int d2) {
    die1 = d1;
    die2 = d2;
}

int Roll::getValue() {
    return die1+die2;
}

bool Roll::isSeven() {
    return (getValue() == 7);
}

bool Roll::isYo() {
    return (getValue() == 11);
}

bool Roll::isBoxcars() {
    return (getValue() == 12);
}

bool Roll::isCraps() {
    return (getValue() == 12) || (getValue() == 2) || (getValue() == 3);
}

void Roll::copyFrom(Roll &src) {
    die1 = src.die1;
    die2 = src.die2;
}

int Roll::countValue(int point) {
    switch (point) {
        case 4:
            return (die1 <= 3) && (die2 <= 3) ? 4 : (die1 >= 4) && (die2 >= 4) ? -4 : 0;
        case 5:
            return (die1 >= 5) && (die2 >= 5) ? -4 : (die1 >= 5) || (die2 >= 5) ? -1 : 2;
        case 6:
            return (die1 == 6) && (die2 == 6) ? -5 : (die1 == 6) || (die2 == 6) ? -2 : 1;
        case 8:
            return (die1 == 1) && (die2 == 1) ? -5 : (die1 == 1) || (die2 == 1) ? -2 : 1;
        case 9:
            return (die1 <= 2) && (die2 <= 2) ? -4 : (die1 <= 2) || (die2 <= 2) ? -1 : 2;
        case 10:
            return (die1 <= 3) && (die2 <= 3) ? -4 : (die1 >= 4) && (die2 >= 4) ? 4 : 0;
        default:
            return 0;
    }
}

int Roll::getDie1() {
    return die1;
}

int Roll::getDie2() {
    return die2;
}

Averager::Averager() {
    total = 0;
    samples = 0;
}

void Averager::add(double value) {
    total += value;
    samples++;
}

double Averager::getValue() {
    return total/(double)samples;
}

Dice::Dice() {}

void Dice::getRoll(Roll &roll) {
    int d1, d2;
    d1 = 1 + 6*(double)(rand()%RAND_MAX)/(double)RAND_MAX;
    d2 = 1 + 6*(double)(rand()%RAND_MAX)/(double)RAND_MAX;
    roll.setDie(d1, d2);
}

void Dice::muckRoll(Roll &roll) {}

CSMDice::CSMDice(int sets, int minDepth) {
    minBufferDepth = minDepth;
    for (int i=0; i<sets; i++) {
        for (int j=1; j<=6; j++) {
            reservoir.push_back(j);
        }
    }
    random_shuffle(reservoir.begin(), reservoir.end());
}

void CSMDice::getRoll(Roll &roll) {
    if (buffer.size() < (2+minBufferDepth)) {
        int numCards = minBufferDepth+2-buffer.size();
        buffer.insert(buffer.end(), reservoir.begin(), reservoir.begin()+numCards);
        reservoir.erase(reservoir.begin(), reservoir.begin()+numCards);
    }
    roll.setDie(buffer[0], buffer[1]);
    buffer.erase(buffer.begin(),buffer.begin()+2);
}

void CSMDice::muckRoll(Roll &roll) {
    reservoir.push_back(roll.getDie1());
    reservoir.push_back(roll.getDie2());
    random_shuffle(reservoir.begin(), reservoir.end());
}

CSM126Dice::CSM126Dice(int sets, int minDepth) : CSMDice(sets, minDepth) {
    for (int i=0; i<sets; i++) {
        for (int j=1; j<=6; j++) {
            muckCard(j);
        }
    }
}

void CSM126Dice::muckCard(int card) {
    int slotNum, pos;
    slotNum = NUM_SLOTS*(double)(rand()%RAND_MAX)/(double)RAND_MAX;
    while (slots[slotNum].size() > MAX_CARDS_PER_SLOT) {
        slotNum = NUM_SLOTS*(double)(rand()%RAND_MAX)/(double)RAND_MAX;
    }
    pos = slots[slotNum].size()*(double)(rand()%RAND_MAX)/(double)RAND_MAX;
    slots[slotNum].insert(slots[slotNum].begin()+pos,card);
}

void CSM126Dice::getRoll(Roll &roll) {
    while (buffer.size() < (2+minBufferDepth)) {
        dropSlot();
    }
    roll.setDie(buffer[0], buffer[1]);
    buffer.erase(buffer.begin(),buffer.begin()+2);
}

void CSM126Dice::dropSlot() {
    int slotNum = NUM_SLOTS*(double)(rand()%RAND_MAX)/(double)RAND_MAX;
    buffer.insert(buffer.end(), slots[slotNum].begin(), slots[slotNum].end());
    slots[slotNum].clear();
}

void CSM126Dice::muckRoll(Roll &roll) {
    muckCard(roll.getDie1());
    muckCard(roll.getDie2());
}