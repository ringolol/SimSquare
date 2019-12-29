#ifndef DG_CONST_H
#define DG_CONST_H

//Main window constants
#define WIN_WIDTH 640
#define WIN_HEIGHT 360
#define WIN_TITLE "SimCyrcle"

//Touch constants
#define SMALL_AREA 0.25

//Timers
#define MAIN_TIMER_UPD_TIME 10.0

//drawable types
enum drawable_types {DRWABLE_STATIC=0,DRWABLE_TOOLS,DRWABLE_WORKSPACE,DRWABLE_BUTTON,DRWABLE_GRAPH};

//drawable class
enum drawable_class {DC_DRAWABLE=0,DC_DRAWCORE,DC_PANEL,DC_WINDOW,DC_GRAPH};

//buttons events types
enum drawable_events {DE_NONE=0,
                      //Workspace window
                      DE_SIM,
                      DE_ADD,
                      DE_BACK,
                      DE_MOVE,
                      DE_SHOWALL,
                      DE_DELETE,
                      //Block type window
                      DE_BT_BACK,
                      DE_BT_COMMON,
                      DE_BT_SOURCE,
                      DE_BT_MATH,
                      DE_BT_LINE,
                      DE_BT_SCOPE,
                      DE_BT_NONLINE,
                      DE_BT_LOGIC,
                      //Block add window
                      DE_AB_BACK,
                      //Main Window
                      DE_MW_NEW,
                      DE_MW_LOAD,
                      DE_MW_REPORT
                      };
//windows
enum window_index {WI_MAIN=0,WI_WS,WI_BT,WI_AB,WI_GR};

//Grid par
//default grid step
extern int GRID_STEP0;
//var grid step
extern int GRID_STEP;
//grid offsets
extern int GRID_OFFSET_X;
extern int GRID_OFFSET_Y;
//graph


#define BTN_SIZE 100

extern double dpi;
#define CONT_OBJ_SIZE 5
#define ZOOM_GAIN 2.0

#define TAP_AND_HOLD_TIME 700
#define VIBRATE_TIME 100
#define DBL_CLK_TIME 300
#define GRAPH_UPD_TIME 400

#define MAX_STR_SIZE 1024

#define DRWCORE_WIDTH 100.0

#define DRWBLE_HEIGHT 150.0

//Graph window
#define NUM_OF_CHAR_X 8

#endif // DG_CONST_H


