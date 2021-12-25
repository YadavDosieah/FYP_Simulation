#ifndef CONFIG_H
#define CONFIG_H

/****** Compilation Flags ******/
#define Analyis_Log 1
// #define Noise_Analysis 1
// #define Post_Eval 1

#define GUI       true
#define Analysis  true
#define Optimise  false

#define logData   false
#define Stop      true

/***** Optimation Parameters *****/
#define No_Of_Trials        20
#define NoOfSteps           750
#define MaxIter             500
#define lambda              20
#define No_Of_Threads       20


/***** Simulation Flags/Variables*****/
#define obj_repulsion true
#define sheep_attraction false

#define Csheep 	        100
#define Cshepherd 	    450
#define Ksheep 	        1.75
#define K1 		        2.0
#define K2 		        1.3
#define init_x          300

#endif //CONFIG_H
