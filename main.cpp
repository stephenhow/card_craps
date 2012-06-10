//
//  main.cpp
//  cardcraps
//
//  Created by Stephen How on 6/2/12.
//  Copyright (c) 2012 DiscountGambling.net. All rights reserved.
//

#include <iostream>
#include <map>

#include "craps.h"
#define MAX_WINDOW_SIZE 6
#define MAX_ODDS 10

void usage() {
    printf("cardcraps [-m minBufferDepth] [-d|-c|-s] [-v] [-w countWindowRolls]\n");
	printf("set minBufferDepth as first argument\n");
	printf("v: verbose (print point stats)\n");
}

void addStats(double value, int count, map<int,Averager> &stats) {
    map<int,Averager>::iterator iter;
    iter = stats.find(count);
    if (iter == stats.end()) {
        stats.insert(pair<int,Averager>(count,Averager())).first->second.add(value);
    } else {
        iter->second.add(value);
    }
}

void printStats(map<int,Averager> &stats) {
    for (map<int,Averager>::iterator iter=stats.begin(); iter!=stats.end(); iter++) {
        printf("%+2d: %+6.4f\n", iter->first, iter->second.getValue());
    }
}

int main(int argc, const char * argv[])
{
    int sets=52, depth=14, window_size=6;
    int c;
    Dice *dice;
    bool verbose=false;
    while ((c = getopt(argc, (char * const *)argv, "hdcsm:vw:")) != EOF) {
        switch (c) {
            case 'm': depth = atoi(optarg); break;
            case 'h': usage(); break;
            case 'd':
				dice = new Dice();
				printf("using normal dice\n");
				break;
            case 'c':
				dice = new CSMDice(sets,depth);
				printf("using CSM with %d dice sets, and minBufferDepth of %d cards\n", sets, depth);
				break;
            case 's':
				dice = new CSM126Dice(sets,depth);
				printf("using model of ShuffleMaster 126 CSM with %d dice sets, and minBufferDepth of %d cards\n", sets, depth);
				break;
            case 'v': verbose = true; break;
            case 'w': window_size = atoi(optarg); break;
        }
    }
	if (!dice) {
		printf("must select dice [-d], CSM [-c], or ShuffleMaster126 CSM [-s]\n");
		usage();
		exit(2);
	}
	printf("using window size of %d rolls\n", window_size);
    // stats
    unsigned long games=0, rolls=0;
    Roll roll;
    
    int point=0, count;
    Roll history[MAX_WINDOW_SIZE];
    int wp=0, passOdds=0, dontOdds=0;
    double pass0x=0, dont0x=0, pass10x=0, dont10x=0, passVarOdds=0, dontVarOdds=0;
    double trueOdds=0;
    // per-count stats
    map<int,Averager> lay4, lay5, lay6, lay8, lay9, lay10;
    unsigned long hist[7];
    for (int i=1; i<=6; i++) hist[i]=0;
    
    while (true) {
        // get the count from the roll history
        count = 0;
        for (int i=0; i<window_size; i++) {
            count += history[i].countValue(point);
        }
        // lay/take odds depending on the count
        switch (point) {
            case 4: case 10:
                dontOdds = (count >= 0 ? MAX_ODDS : 0);
                passOdds = (count < 0 ? MAX_ODDS : 0);
                break;
            case 5: case 9:
                dontOdds = (count > 0 ? MAX_ODDS : 0);
                passOdds = (count < 0 ? MAX_ODDS : 0);
                break;
            case 6: case 8:
                dontOdds = (count > -3 ? MAX_ODDS : 0);
                passOdds = (count <= -3 ? MAX_ODDS : 0);
                break;
        }
        // roll the dice
        dice->getRoll(roll);
        hist[roll.getDie1()]++;
        trueOdds = (point == 10) || (point == 4) ? 2.0 :
                    (point == 5) || (point == 9) ? 1.5 :
                    6.0/5.0;
        if (!point) {
            // coming out
            if (roll.isYo() || roll.isSeven()) {
                pass0x += 1;
                pass10x += 1;
                passVarOdds += 1;
                dont0x -= 1;
                dont10x -= 1;
                dontVarOdds -= 1;
                games++;
            } else if (roll.isBoxcars()) {
                // bar the donts
                pass0x -= 1;
                pass10x -= 1;
                passVarOdds -= 1;
                games++;
            } else if (roll.isCraps()) {
                pass0x -= 1;
                pass10x -= 1;
                passVarOdds -= 1;
                dont0x += 1;
                dont10x += 1;
                dontVarOdds += 1;
                games++;
            } else {
                point = roll.getValue();
            }
        } else {
            if (roll.isSeven()) {
                // seven out
                pass0x -= 1;
                pass10x -= (1 + 10);
                passVarOdds -= (1 + passOdds);
                dont0x += 1;
                dont10x += (1 + 10/trueOdds);
                dontVarOdds += (1 + dontOdds/trueOdds);
                switch (point) {
                    case 4:  addStats(0.5, count, lay4); break;
                    case 5:  addStats(2.0/3.0, count, lay5); break;
                    case 6:  addStats(5.0/6.0, count, lay6); break;
                    case 8:  addStats(5.0/6.0, count, lay8); break;
                    case 9:  addStats(2.0/3.0, count, lay9); break;
                    case 10: addStats(0.5, count, lay10); break;
                }
                games++;
                point = 0;
            } else if (roll.getValue() == point) {
                // made point
                pass0x += 1;
                pass10x += (1 + 10*trueOdds);
                passVarOdds += (1 + passOdds*trueOdds);
                dont0x -= 1;
                dont10x -= (1 + 10);
                dontVarOdds -= (1 + dontOdds);
                switch (point) {
                    case 4:  addStats(-1.0, count, lay4); break;
                    case 5:  addStats(-1.0, count, lay5); break;
                    case 6:  addStats(-1.0, count, lay6); break;
                    case 8:  addStats(-1.0, count, lay8); break;
                    case 9:  addStats(-1.0, count, lay9); break;
                    case 10: addStats(-1.0, count, lay10); break;
                }
                games++;
                point = 0;
            }
        }
        history[wp].copyFrom(roll);
        wp = (wp+1)%window_size;
        if ((point == 0) && games%1000000 == 0) {
            printf("%ld games:\npass flat: %+6.4f, pass10x: %+6.4f, pass w/count: %+6.4f,\ndont flat: %+6.4f, dont10x: %+6.4f, dont w/count: %+6.4f\n",
                   games,
                   pass0x/(double)games, pass10x/(double)games, passVarOdds/(double)games,
                   dont0x/(double)games, dont10x/(double)games, dontVarOdds/(double)games);
            if (verbose) {
                printf("\npoint 4 lay vs. count:\n");
                printStats(lay4);
                printf("\n");
                printf("point 5 lay vs. count:\n");
                printStats(lay5);
                printf("\n");
                printf("point 6 lay vs. count:\n");
                printStats(lay6);
                printf("\n");
            }
        }
        dice->muckRoll(roll);
		if (false && !dice->countdown()) {
			printf("roll: %ld\n", rolls);
			exit(-1);
		}
        rolls++;
    }
    
    //delete dice;
    return 0;
}

