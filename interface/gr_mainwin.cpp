#include "gr_mainwin.h"


MainWindow::MainWindow()
{
    //Show overlay
    //InfOverlay=true;

    //android

    #if defined(Q_OS_ANDROID)
    Gr_Logger().Get()<<"Setup android dir & touch events";
    QAndroidJniObject activity=QtAndroid::androidActivity();
    QAndroidJniObject context=QtAndroid::androidContext();
    QAndroidJniObject str_dir = QAndroidJniObject::fromString("");
    QAndroidJniObject AppDirJni = activity.callObjectMethod("getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;",str_dir.object<jstring>());
    if(AppDirJni.toString() == QString(""))
        AppDirJni=activity.callObjectMethod("getFilesDir", "()Ljava/io/File;");
    AppDir=AppDirJni.toString();
    //set attr
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    //grab gesture
    this->grabGesture(Qt::GestureType::PinchGesture);
    #else
    Gr_Logger().Get()<<"Setup win dir";
    AppDir=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    #endif
    AppDir+='/';

    //set up vibration
    SetVibrator();

    //set window pars
    //Gr_Logger().Get()<<"Set MainWin size";
    //SetWindow();

    //Update screen size
    Gr_Logger().Get()<<"The first screen size Upd";
    UpdSizeInf();

    //set grid
    SetGrid();

    //load widndows/panels/blocks/buttons
    LoadContent();

    //create pens fonts etc.
    SetPainters();

    //set screen refresh timer
    SetMainTimer();

    //setup dbl clk timer
    SetDblClkTimer();

    //set tap and hold timer
    SetTapHoldTimer();

    //graph update timer
    SetGraphTimer();
}

MainWindow::MainWindow(QApplication *a) : MainWindow()
{
    Gr_Logger().Get()<<"MainWin constructor";
    Gr_Logger().Get()<<"Set Quit slots";
    //Save QApp & setup quit slots
    ThisApp=a;
    connect(a,SIGNAL(applicationStateChanged(Qt::ApplicationState)),this,SLOT(AppStateEv(Qt::ApplicationState)));
    connect(a,SIGNAL(aboutToQuit()),this,SLOT(AboutToQuit()));
}

MainWindow::~MainWindow()
{
    Gr_Logger().Get()<<"MainWin destructor";
    CoreThread.exit();
    CoreThread.wait();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    //draw preparation
    painter.begin(this);

    //set default pens/brushes/fonts and render options
    SetDrawInstr();

    //draw things
    Draw(event);

    //end painting
    painter.end();
}

void MainWindow::Draw(QPaintEvent *event)
{
    //upd
    UpdSizeInf();
    Windows[ActiveWin]->Upd(w_w,w_h);

    //draw BG
    painter.fillRect(event->rect(), bgBrush);

    //draw workspace
    if(ActiveWin==WI_WS)
        DrawWorkSpace(event);

    //draw GUI
    Windows[ActiveWin]->Draw(&painter,event);

    //draw information overlay
    DrawInf(event);
}

void MainWindow::DrawWorkSpace(QPaintEvent *event)
{
    //draw grid
    DrawGrid(event);

    //draw objects
    DrawObjects(event);

    //draw lines
    DrawLines(event);

    //
    DrawInpHL(event);
}

/*void MainWindow::SetWindow()
{
    QScreen *screen = QApplication::screens().at(0);
    resize(screen->availableSize().width(),screen->availableSize().height());
}*/

void MainWindow::SetDrawInstr()
{
    //set default instruments
    painter.setPen(mainPen);
    painter.setFont(mainFont);
    //set render hints
    painter.setRenderHint(QPainter::Antialiasing);
}

void MainWindow::SetPainters()
{
    Gr_Logger().Get()<<"Set main painters, brushes & pens";
    //backgroud color
    bgBrush = QBrush(QColor(255, 255, 255));
    //main pen
    mainPen = QPen(Qt::black);
    mainPen.setWidth(1);
    //grid pen
    gridPen=QPen(Qt::lightGray);
    gridPen.setWidth(1);
    //line pen
    linePen=QPen(Qt::black);
    linePen.setWidth((int)(0.1*Get1cm()+0.5));
    //linePen.setWidth(3);
    //main text font
    mainFont.setPixelSize(12);
}

void MainWindow::SetMainTimer()
{
    Gr_Logger().Get()<<"Set screen refresh timer";
    //set and start the main timer
    mainTimer= new QTimer(this);
    connect(mainTimer, &QTimer::timeout, this, &MainWindow::Anima);
    mainTimer->start(MAIN_TIMER_UPD_TIME);
}

void MainWindow::UpdSizeInf()
{
    QScreen *screen = QApplication::screens().at(0);
    w_w=size().width();
    w_h=size().height();
    dpi=screen->logicalDotsPerInch();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    //call original event
    QOpenGLWidget::resizeEvent(event);
    //force graph redraw -- reset graph
    Graph->isPrepared=false;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Back)
    {
        Gr_Logger().Get()<<"BackKey pressed; win: "<<WinToString(ActiveWin);
        switch (ActiveWin)
        {
        case WI_WS:
            ChangeWindow(WI_MAIN);
            break;
        case WI_MAIN:
            ThisApp->exit();
            break;
        case WI_AB:
            ChangeWindow(WI_BT);
            break;
        case WI_GR:
            ChangeWindow(WI_WS);
            break;
        case WI_BT:
            ChangeWindow(WI_WS);
            break;
        }
        event->accept();
    }
}

void MainWindow::Anima()
{
    //drag object
    if(isDrag)
    {
        Drawables[drag_ind]->SetX(mX+drag_offset_X);
        Drawables[drag_ind]->SetY(mY+drag_offset_Y);
    }

    //update grid offset
    if(isOffset)
    {
        //for workspace
        if(ActiveWin==WI_WS)
        {
            //upd
            grid_offset_smoth_X+=mX-Xoff1;
            grid_offset_smoth_Y+=mY-Yoff1;
            UpdGrid();
            //store mouse position for next iteration
            Xoff1=mX;
            Yoff1=mY;
        }
        //for graph
        else if(ActiveWin==WI_GR)
        {
            //upd size
            double lx,ly;
            lx=Graph->xmax-Graph->xmin;
            ly=Graph->ymax-Graph->ymin;
            double dx,dy;
            dx=(mX-Xoff1)*lx/Graph->gra_w;
            dy=(mY-Yoff1)*ly/Graph->gra_h;
            Graph->xmin-=dx;
            Graph->xmax-=dx;
            Graph->ymin+=dy;
            Graph->ymax+=dy;
            //recalc data for graph
            Graph->Prepare(false);
            //store mouse position for next iteration
            Xoff1=mX;
            Yoff1=mY;
        }
    }

    //check if user start drag object
    if(isHold && sqrt((mX-mX0)*(mX-mX0)+(mY-mY0)*(mY-mY0))>Get1cm()*SMALL_AREA)
    {
        Gr_Logger().Get()<<"Drag started; win: "<<WinToString(ActiveWin);
        //if they started than
        StopTapHoldTimer();
        //ClearHL();
        SetIsHold(false);
        ModifyObject(NULL,ind_pressed,3);
    }

    //check if user start scrolling
    if(may_scroll)
    {
        if(!scroll && sqrt((mX-mX0)*(mX-mX0)+(mY-mY0)*(mY-mY0))>Get1cm()*SMALL_AREA)
        {
            Gr_Logger().Get()<<"Scroll started; win: "<<WinToString(ActiveWin);
            Yscr=mY;
            scroll=true;
        }
    }
    //animate scrolling
    if(scroll)
    {
        int dy=mY-Yscr;
        int yn=Windows[ActiveWin]->Panels[clk.first]->y+dy;
        if((dy>0 && yn<Windows[ActiveWin]->Panels[clk.first]->y0) || (dy<0 && yn+Windows[ActiveWin]->Panels[clk.first]->height>w_h))
        {
            Windows[ActiveWin]->Panels[clk.first]->y+=dy;
        }
        Yscr=mY;
    }

    //redraw window (call paintEvent)
    update();
}

void MainWindow::SetGrid()
{
    Gr_Logger().Get()<<"Set default grid size";
    GRID_STEP0=(int)(Get1cm()*0.75+0.5);
    GRID_STEP=GRID_STEP0;
    GRID_OFFSET_X=0;
    GRID_OFFSET_Y=0;
    grid_step_smooth=GRID_STEP0;
    grid_offset_smoth_X=GRID_OFFSET_X;
    grid_offset_smoth_Y=GRID_OFFSET_Y;
}

void MainWindow::SetDblClkTimer()
{
    Gr_Logger().Get()<<"Set double-click timer";
    doubleClkTimer = new QTimer(this);
    doubleClkTimer->setSingleShot(true);
    connect(doubleClkTimer, &QTimer::timeout, this, &MainWindow::NotDoubleClk);
}

void MainWindow::SetTapHoldTimer()
{
    Gr_Logger().Get()<<"Set tap&hold timer";
    tapHoldTimer = new QTimer(this);
    tapHoldTimer->setSingleShot(true);
    connect(tapHoldTimer, &QTimer::timeout, this, &MainWindow::TapAndHoldEv);
}

void MainWindow::SetGraphTimer()
{
    Gr_Logger().Get()<<"Set graph update timer";
    graphUpd= new QTimer(this);
    connect(graphUpd, &QTimer::timeout, this, &MainWindow::PrepareGra);
}

void MainWindow::SetVibrator()
{
    #if defined(Q_OS_ANDROID)
    Gr_Logger().Get()<<"Set vibrator";
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("Vibrator", &vibrator);
    #endif
}

void MainWindow::DrawInf(QPaintEvent *event)
{
    if(!InfOverlay)
        return;

    //set Pen
    QFont ov_font;
    ov_font.setPixelSize((int)(0.5*Get1cm()+0.5));
    painter.setFont(ov_font);
    //create string
    QString str=QString("\nmX=")+QString::number(mX)\
            +QString(";   mY=")+QString::number(mY)\
            +QString("\nWinW=")+QString::number(this->width())\
            +QString("   WinH=")+QString::number(this->height())\
            +QString("\nGRID_STEP=")+QString::number(grid_step_smooth)\
            +QString("\nGRID_X=")+QString::number(grid_offset_smoth_X)\
            +QString(";   GRID_Y=")+QString::number(grid_offset_smoth_X)\
            +"\ndir=\""+AppDir+"\""\
            +"\nfile_name=\""+file_name+"\""\
            +QString("\nCheck Point=")+QString::number(testCheckpoint)+\
            testStr +'\n';
    //draw overlay
    painter.drawText(QRect(0, 0, w_w, w_h), Qt::AlignLeft,str);
}

void MainWindow::DrawObjects(QPaintEvent *event)
{
    painter.setPen(linePen);
    for(int i=0;i<Drawables.size();i++)
    {
            Drawables[i]->Draw(&painter,event);
    }
}

void MainWindow::DrawLines(QPaintEvent *event)
{
    painter.setPen(linePen);

    for(int i=0;i<Lines.size();i++)
    {
        Lines[i]->Draw(&painter,event);
    }
}

void MainWindow::DrawInpHL(QPaintEvent *event)
{
    ClearPutsHL();
    if(DotInExist)
    {
        SetPutsHL(line_indIn, 1,line_numIn);
    }
    else if(DotOutExist)
    {
        SetPutsHL(line_indOut, 2,line_numOut);
    }
    //
}

void MainWindow::DrawGrid(QPaintEvent *event)
{
    //set pen
    painter.setPen(gridPen);
    //draw vertical lines
    int x_off=GRID_OFFSET_X%GRID_STEP;
    int y_off=GRID_OFFSET_Y%GRID_STEP;
    for (int i=0;i<event->rect().width();i++)
    {
        painter.drawLine(i*GRID_STEP+x_off,0,i*GRID_STEP+x_off,event->rect().height());
    }
    //draw horizontal lines
    for (int j=0;j<event->rect().height();j++)
    {
        painter.drawLine(0,j*GRID_STEP+y_off,event->rect().width(),j*GRID_STEP+y_off);
    }
}

//Mouse
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    MouMoveEv(event->x(),event->y());
}

/*void MainWindow::CopyByDragging(QMouseEvent *event, int i)
{
    Gr_Logger().Get()<<"Copy by dragging; win: "<<WinToString(ActiveWin);
    //create new item
    Gr_DrawCore *dr=new Gr_DrawCore(Drawables[i]);
    //set workspace-type
    dr->SetType(DRWABLE_WORKSPACE);
    //set calc index
    dr->calcInd=GetCalcInd();
    //add element to drawables
    Drawables.push_back(dr);
    //set drag options
    drag_ind=Drawables.size()-1;
    isDrag=1;
    drag_offset_X=Drawables[i]->GetX()-mX-GRID_OFFSET_X;
    drag_offset_Y=Drawables[i]->GetY()-mY-GRID_OFFSET_Y;
}*/

void MainWindow::WorkSpaceClick()
{
    Gr_Logger().Get()<<"WorkSpace click; win: "<<WinToString(ActiveWin);
    //else offset grid
    Xoff1=mX;
    Yoff1=mY;
    isOffset=true;
}

void MainWindow::ModifyObject(QMouseEvent *event, int i,int force_CT)
{
    Gr_Logger().Get()<<"Modify. Object; win: "<<WinToString(ActiveWin)<<"; obj_ind: "<<i;
    if(i==-1)
    {
        Gr_Logger().Get(LogError)<<"Modify. Incorect object index; win: "<<WinToString(ActiveWin);
        return;
    }

    //force cursor type
    int c_type=cursor_type;
    if(force_CT!=-1)
        c_type=force_CT;

    Gr_Logger().Get()<<"Modify. Cursor type: "<<c_type;
    switch (c_type)
    {
    case 0:
        Gr_Logger().Get()<<"Modify. Single click";
        int InpCl;
        int OutCl;
        //Is input clicked?
        if((InpCl=Drawables[i]->InputClicked(mX,mY))!=-1)
        {
            if(line_indIn==i && line_numIn==InpCl)
                DotInExist=!DotInExist;
            else
                DotInExist=true;

            Gr_Logger().Get()<<"Modify. Input was clicked; flag: "<<DotInExist;
            line_indIn=i;
            line_numIn=InpCl;

            //HL HERE
            //Drawables[line_indIn]->SetHL(DotInExist?1:0,line_numIn);
        }
        //Is output clicked?
        else if((OutCl=Drawables[i]->OutputClicked(mX,mY))!=-1)
        {
            if(line_indOut==i && line_numOut==OutCl)
                DotOutExist=!DotOutExist;
            else
                DotOutExist=true;

            Gr_Logger().Get()<<"Modify. Output is ckicked; flag: "<<DotOutExist;

            line_indOut=i;
            line_numOut=OutCl;

            //HL HERE
            //Drawables[line_indOut]->SetHL(DotOutExist?2:0,line_numOut);
        }
        else
        {
            //Toggle object HL
            ToggleHL(i);
        }
        //create line between input and output if both flags are set
        if(DotInExist && DotOutExist)
        {
            Gr_Logger().Get()<<"Modify. Create link";
            Gr_Line* linet=new Gr_Line();
            linet->SetPi(Drawables[line_indIn],line_numIn);
            linet->SetPo(Drawables[line_indOut],line_numOut);
            Lines.push_back(linet);
            //Drawables[line_indIn]->ResetHL();
            //Drawables[line_indOut]->ResetHL();
            DotInExist=false;
            DotOutExist=false;
        }
        break;
    case 1:
        Gr_Logger().Get()<<"Modify. Double click -- rotate block";
        //rotate block
        Drawables[i]->Rotate();
        ClearHL();
        break;
    case 2:
        Gr_Logger().Get()<<"Modify. Click & hold";
        if(Drawables[i]->GetType()==DRWABLE_WORKSPACE)
        {
            //if graph was clicked
            if(Drawables[i]->core_type==COT_SCP || Drawables[i]->core_type==COT_SCP_XY || Drawables[i]->core_type==COT_SCP_4PLOTS)
            {
                Gr_Logger().Get()<<"Modify. Graph was clicked";
                //vibrate
                #if defined(Q_OS_ANDROID)
                vibrator.vibrate(VIBRATE_TIME);
                #endif

                //prepare graph
                Gr_Logger().Get()<<"Modify. Set data & prepare Graph";
                Graph->SetData(&Drawables[i]->data);
                Graph->Prepare();
                if(CoreThread.isRunning())
                {
                    Gr_Logger().Get()<<"Modify. Start updating graph";
                    graphUpd->start(GRAPH_UPD_TIME);
                }
                //pop-up graph
                Gr_Logger().Get()<<"Modify. Show graph";
                ChangeWindow(WI_GR);
            }
            //if another block was clicked
            else
            {
                Gr_Logger().Get()<<"Modify. Non-graph was clicked";
                //Show modal with parameters
                SetSAndP(Drawables[i]);
            }
        }
        ClearHL();
        break;
    case 3:
        Gr_Logger().Get()<<"Modify. Start dragging workspace/graph";
        //set and start dragging workspace/graph
        drag_ind=i;
        isDrag=2;
        drag_offset_X=Drawables[i]->GetX()-mX-GRID_OFFSET_X;
        drag_offset_Y=Drawables[i]->GetY()-mY-GRID_OFFSET_Y;
        ClearHL();
        break;
    }
}

void MainWindow::ButtonClick(QMouseEvent *event, Gr_Drawable *o)
{
    Gr_Logger().Get()<<"Button was clicked; win: "<<WinToString(ActiveWin);
    if(ActiveWin==WI_WS)
    {
        switch (o->event)
        {
        case DE_SIM:
            Gr_Logger().Get()<<"SimBtn clicked";
            StartCalc();
            break;
        case DE_ADD:
            Gr_Logger().Get()<<"AddBtn clicked";
            ChangeWindow(WI_BT);
            ClearHL();
            break;
        case DE_DELETE:
            Gr_Logger().Get()<<"DelBtn clicked";
            DeleteObjs();
            ClearHL();
            break;
        case DE_SHOWALL:
            Gr_Logger().Get()<<"ShowAllBtn clicked";
            break;
        case DE_BACK:
            Gr_Logger().Get()<<"BackBtn clicked";
            ChangeWindow(WI_MAIN);
        }
    }
    else if(ActiveWin==WI_BT)
    {
        if(o->event==DE_BT_BACK)
        {
            Gr_Logger().Get()<<"BackBtn clicked";
            ChangeWindow(WI_WS);
        }
        else
        {
            Gr_Logger().Get()<<"Block Type clicked";
            Gr_Logger().Get()<<"Hide all block types panels";
            ChangeWindow(WI_AB);
            for(int i=1;i<Windows[ActiveWin]->Panels.size();i++)
            {
                Windows[ActiveWin]->Panels[i]->isDrawn=false;
            }

            Gr_Logger().Get()<<"Show the relevant BT panel";
            int ind=-1;
            switch (o->event)
            {
            case DE_BT_COMMON:
                ind=1;
                break;
            case DE_BT_SOURCE:
                ind=2;
                break;
            case DE_BT_MATH:
                ind=5;
                break;
            case DE_BT_LINE:
                ind=6;
                break;
            case DE_BT_SCOPE:
                ind=3;
                break;
            case DE_BT_NONLINE:
                ind=7;
                break;
            case DE_BT_LOGIC:
                ind=4;
                break;
            }

            if(ind>0)
                Windows[ActiveWin]->Panels[ind]->isDrawn=true;

        }
    }
    else if(ActiveWin==WI_AB)
    {
        if(o->event==DE_AB_BACK)
        {
            Gr_Logger().Get()<<"BackBtn clicked";
            ChangeWindow(WI_BT);
            return;
        }
        else
        {
            Gr_Logger().Get()<<"Block clicked -- add the block to WS";
            AddBlock(((Gr_DrawCore*)o)->core_type);
        }

        ChangeWindow(WI_WS);
    }
    else if(ActiveWin==WI_MAIN)
    {
        switch (o->event)
        {
        case DE_MW_NEW:
            Gr_Logger().Get()<<"New file btn clicked";
            Gr_Logger().Get()<<"Save current project";
            Save();
            Gr_Logger().Get()<<"Clear workspace";
            Clear();
            Gr_Logger().Get()<<"Create a new project";
            if(Save(true))
                ChangeWindow(WI_WS);
            break;
        case DE_MW_LOAD:
            Gr_Logger().Get()<<"LoadBtn clicked";
            Gr_Logger().Get()<<"Save current project";
            Save();
            Gr_Logger().Get()<<"Load a project";
            if(Load())
                ChangeWindow(WI_WS);
            break;
        case DE_MW_REPORT:
            Gr_Logger().Get()<<"ReportBtn clicked";
            break;
        }
    }
    else if(ActiveWin==WI_GR)
    {
        switch (o->event)
        {
        case DE_BACK:
            Gr_Logger().Get()<<"BackBtn clicked";
            ChangeWindow(WI_WS);
        }
    }
}

void MainWindow::WorkspaceObjClick(QMouseEvent *event)
{
    Gr_Logger().Get()<<"Workspace Object clicked";

    //click on a line - highlight it
    for(int i=0;i<Lines.size();i++)
    {
        if(Lines[i]->Clicked(mX,mY))
        {
            Gr_Logger().Get()<<"Line by index: "<<i<<" was clicked";
            Lines[i]->ToggleHL();
            //return;
        }
    }

    for(int i=Drawables.size()-1;i>=0;i--)
    {
        //if the i-th element is clicked
        if(Drawables[i]->Clicked(mX,mY))
        {
            Gr_Logger().Get()<<"Drawable clicked; ind: "<< i;
            switch(Drawables[i]->GetType())
            {
            //worckspace items handler
            case DRWABLE_WORKSPACE:
                Gr_Logger().Get()<<"Drawable_workspace clicked";
                //set options to handle click(on release) or hold events
                //mem index
                SetIndPressed(i);
                //set flag (if !isHold than Drag or Tap&Hold ev)
                SetIsHold(true);

                if(isBodyClick(i))
                {
                    Gr_Logger().Get()<<"Drawable's body was clicked";
                    //set double clk flag
                    if(DoubleClkFlg && RotateInd==i)
                    {
                        Gr_Logger().Get()<<"Set rotate flag";
                        DblRotate=true;
                    }
                    else
                    {
                        /*if(DoubleClkFlg)
                        {
                            Gr_Logger().Get()<<"Single click handler";
                            ModifyObject(NULL,RotateInd,0);
                        }*/
                        Gr_Logger().Get()<<"Set rotate ind & double click flag";
                        //start double clk
                        RotateInd=i;
                        DoubleClkFlg=true;
                        //set HL
                        //Drawables[ind_pressed]->wasClicked=!Drawables[ind_pressed]->wasClicked;
                        Gr_Logger().Get()<<"Start double click timer";
                        //start timer
                        doubleClkTimer->start(DBL_CLK_TIME);
                    }
                    Gr_Logger().Get()<<"Start tap & hold timer";
                    //start Tap&Hold timer
                    tapHoldTimer->start(TAP_AND_HOLD_TIME);
                }
                break;
            }
            return;
        }
    }
    //else offset grid
    if(cursor_type==0)
        WorkSpaceClick();
}

bool MainWindow::event(QEvent *event)
{
    //if gesture
    if(event->type()==QEvent::Gesture)
    {
        QGestureEvent *ev=(QGestureEvent*)event;
        //if gesture is pinch
        if(QPinchGesture *pinch=(QPinchGesture*)ev->gesture(Qt::PinchGesture))
        {
            double sc=pinch->scaleFactor();
            if(sc>=1)
                sc=1+(sc-1)/ZOOM_GAIN;
            else
                sc=1/(1+(1/sc-1)/ZOOM_GAIN);

            ZoomEv(pinch->centerPoint().x(),pinch->centerPoint().y(),sc);
        }
        //Looks like i dont need that(?)
        //ResetFlags();
        return true;
    }
    //accept new gestures
    else  if(event->type() == QEvent::GestureOverride)
    {
        event->accept();
    }
    //tap events
    else if(event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchEnd || event->type() == QEvent::TouchCancel || event->type()==QEvent::TouchUpdate)
    {
        QTouchEvent *ev=(QTouchEvent*)event;

        if(ev->touchPoints().size()==0) return true;

        int px=ev->touchPoints()[0].lastPos().x();
        int py=ev->touchPoints()[0].lastPos().y();

        //pressed event
        if(ev->touchPoints()[0].state()==Qt::TouchPointState::TouchPointPressed)
        {
            Gr_Logger().Get()<<"Gesture press; win: "<<WinToString(ActiveWin);
            MouPressEv(px,py);
        }
        //release event
        else if (ev->touchPoints()[0].state()==Qt::TouchPointState::TouchPointReleased)
        {
            Gr_Logger().Get()<<"Gesture release; win: "<<WinToString(ActiveWin);
            MouReleaseEv(px,py);
        }
        //moved event
        else if(ev->touchPoints()[0].state()==Qt::TouchPointState::TouchPointMoved)
        {
            MouMoveEv(px,py);
        }
        return true;
    }
    return QOpenGLWidget::event(event);
}

void MainWindow::TapAndHoldEv()
{
    Gr_Logger().Get()<<"Tap&Hold event; win: "<<WinToString(ActiveWin);
    //ClearHL();
    ResetDblClk();
    SetIsHold(false);
    ModifyObject(NULL,ind_pressed,2);
    //this is called only after release ev, because of modal
    ResetFlags();
}

void MainWindow::NotDoubleClk()
{
    //HL HERE
    Gr_Logger().Get()<<"Not double click; win: "<<WinToString(ActiveWin);
    /*if(!isDrag)
    {
        Gr_Logger().Get()<<"Single click handler";
        ModifyObject(NULL,RotateInd,0);
    }*/
    ResetDblClk();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    Gr_Logger().Get()<<"Mouse press event; win: "<<WinToString(ActiveWin);
    MouPressEv(event->x(),event->y());
}

void MainWindow::MouPressEv(int px,int py)
{
    Gr_Logger().Get()<<"Mouse press handler; win: "<<WinToString(ActiveWin);

    Gr_Logger().Get()<<"Upd mouse position & memo original position";

    //update mouse position
    mX=px;
    mY=py;

    //mem original position
    mX0=mX;
    mY0=mY;

    //reset mouse flags and vars
    ResetFlags();

    Gr_Logger().Get()<<"Check panel click";
    //check panel click
    clk=Windows[ActiveWin]->ObjClicked(mX,mY);

    //if any panel was clicked
    if(clk.first!=-1)
    {
        Gr_Logger().Get()<<"Panel clicked; ind: "<< clk.first;
        if(Windows[ActiveWin]->Panels[clk.first]->resize_cont)
        {
            Gr_Logger().Get()<<"Click was on a scrollable panel -- set may_scroll flag true";
            //can be scrolled
            may_scroll=true;
        }
        return;
    }

    Gr_Logger().Get()<<"Click was on work zone";

    //if click was on the work zone
    switch(ActiveWin)
    {
    //workspace window
    case WI_WS:
        Gr_Logger().Get()<<"Click was on WorkSpaceWin";
        WorkspaceObjClick(NULL);
        break;
    //graph window
    case WI_GR:
        Gr_Logger().Get()<<"Click was on GraphWin";
        WorkSpaceClick();
        break;
    }
}

void MainWindow::MouMoveEv(int px,int py)
{
    //update mouse position information
    mX=px;
    mY=py;
}

void MainWindow::MouReleaseEv(int px,int py)
{
    Gr_Logger().Get()<<"Release mouse handler";
    //update m pos
    mX=px;
    mY=py;

    //double click
    if(DblRotate)
    {
        Gr_Logger().Get()<<"Double click handler";
        //ClearHL();
        ModifyObject(NULL,RotateInd,1);
        ResetDblClk();
        //feature... when you double click you also exc single_click_handler
        ResetFlags();
        return;
    }

    //click object
    if(isHold)
    {
        Gr_Logger().Get()<<"Single click handler";
        ModifyObject(NULL,ind_pressed,0);
    }

    //press btn on panel
    if(!scroll && clk.second!=-1)
    {
        Gr_Logger().Get()<<"Panel button handler";
        //handle event
        ButtonClick(NULL,Windows[ActiveWin]->Panels[clk.first]->Objects[clk.second]);
    }

    //reset mouse flags
    ResetFlags();
}

void MainWindow::ZoomEv(int px,int py,double sc)
{
    isDrag=false;
    if(ActiveWin==WI_WS)
    {
        //zoom in/out on cursor position (workspace)
        double grid_s_new;
        grid_s_new=grid_step_smooth*sc;
        grid_s_new=grid_s_new>1.0?grid_s_new:1.0;

        grid_offset_smoth_X=(grid_offset_smoth_X)*(grid_s_new/grid_step_smooth)+px-px*(grid_s_new/grid_step_smooth);
        grid_offset_smoth_Y=(grid_offset_smoth_Y)*(grid_s_new/grid_step_smooth)+py-py*(grid_s_new/grid_step_smooth);
        grid_step_smooth=grid_s_new;

        UpdGrid();
    }
    else if(ActiveWin==WI_GR)
    {
        double lx,ly;
        lx=Graph->xmax-Graph->xmin;
        ly=Graph->ymax-Graph->ymin;
        double tx,ty;
        sc=1/sc;
        tx=(px-Graph->gra_x)*lx/Graph->gra_w+Graph->xmin;
        ty=(Graph->gra_h-(py-Graph->gra_y))*ly/Graph->gra_h+Graph->ymin;

        Graph->xmin=Graph->xmin*sc+tx-tx*sc;
        Graph->ymin=Graph->ymin*sc+ty-ty*sc;
        Graph->xmax=Graph->xmin+lx*sc;
        Graph->ymax=Graph->ymin+ly*sc;

        Graph->Prepare(false);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Gr_Logger().Get()<<"Mouse release event; win: "<<WinToString(ActiveWin);
    MouReleaseEv(event->x(),event->y());
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    ZoomEv(event->x(),event->y(),1+0.1*sign(event->angleDelta().y()));
}

void MainWindow::LoadContent()
{
    Gr_Logger().Get()<<"Loading content...";

    //create windows
    Gr_Logger().Get()<<"Load cont. Set windows";
    //workspace
    //forse rescale by setting width-1
    Gr_Window *gui_ws=new Gr_Window(0,0,w_w-1,w_h,DRWABLE_STATIC);
    //block type
    Gr_Window *gui_bt=new Gr_Window(0,0,w_w-1,w_h,DRWABLE_STATIC);
    //add block
    Gr_Window *gui_ab=new Gr_Window(0,0,w_w-1,w_h,DRWABLE_STATIC);
    //graph
    Graph=new Gr_Graph(0,0,w_w-1,w_h,DRWABLE_GRAPH);
    //main
    Gr_Window *gui_main=new Gr_Window(0,0,w_w-1,w_h,DRWABLE_STATIC);
    Windows.insert(pair<window_index,Gr_Window*>(WI_WS,gui_ws));
    Windows.insert(pair<window_index,Gr_Window*>(WI_BT,gui_bt));
    Windows.insert(pair<window_index,Gr_Window*>(WI_AB,gui_ab));
    Windows.insert(pair<window_index,Gr_Window*>(WI_GR,Graph));
    Windows.insert(pair<window_index,Gr_Window*>(WI_MAIN,gui_main));


    //WORKSPACE WINDOW
    Gr_Logger().Get()<<"Load cont. Set WorkSpace win";
    //toolbar
    Gr_Panel *p1=new Gr_Panel(w_w-Get1cm()*5,0,Get1cm()*5,Get1cm()*1.5);
    p1->right=true;
    p1->top=true;
    p1->LoadImg(":/images/toolbar/pic/toolbar_bg.png");
    Windows[WI_WS]->Panels.push_back(p1);
    //sim button
    Gr_Drawable *sim_btn=new Gr_Drawable(Get1cm()*0.5,Get1cm()*0.25,Get1cm(),Get1cm(),DRWABLE_BUTTON,DE_SIM);
    sim_btn->LoadImg(":/images/elements/pic/butt_sim.png");
    Windows[WI_WS]->Panels[0]->Objects.push_back(sim_btn);
    //show all
    Gr_Drawable *showall_btn=new Gr_Drawable(Get1cm()*2,Get1cm()*0.25,Get1cm(),Get1cm(),DRWABLE_BUTTON,DE_SHOWALL);
    showall_btn->LoadImg(":/images/toolbar/pic/showall_btn.png");
    Windows[WI_WS]->Panels[0]->Objects.push_back(showall_btn);
    //delete btn
    Gr_Drawable *delete_btn=new Gr_Drawable(Get1cm()*3.5,Get1cm()*0.25,Get1cm(),Get1cm(),DRWABLE_BUTTON,DE_DELETE);
    delete_btn->LoadImg(":/images/toolbar/pic/delete_btn.png");
    Windows[WI_WS]->Panels[0]->Objects.push_back(delete_btn);


    //back panel
    Gr_Panel *p2=new Gr_Panel(0,0,Get1cm()*1.5,Get1cm()*1.5);
    p2->left=true;
    p2->top=true;
    p2->LoadImg(":/images/toolbar/pic/arrow_bg.png");
    Windows[WI_WS]->Panels.push_back(p2);
    //back btn
    Gr_Drawable *back_btn=new Gr_Drawable(0,0,Get1cm()*1.5,Get1cm()*1.5,DRWABLE_BUTTON,DE_BACK);
    back_btn->LoadImg(":/images/toolbar/pic/back_btn.png");
    Windows[WI_WS]->Panels[1]->Objects.push_back(back_btn);

    //add panel
    Gr_Panel *p3=new Gr_Panel(w_w-Get1cm()*1.75,w_h-Get1cm()*1.75,Get1cm()*1.75,Get1cm()*1.75);
    p3->right=true;
    p3->bottom=true;
    Windows[WI_WS]->Panels.push_back(p3);
    //add button
    Gr_Drawable *add_btn=new Gr_Drawable(0,0,Get1cm()*1.5,Get1cm()*1.5,DRWABLE_BUTTON,DE_ADD);
    add_btn->LoadImg(":/images/toolbar/pic/btn_add.png");
    Windows[WI_WS]->Panels[2]->Objects.push_back(add_btn);


    //BLOCKTYPE WINDOW
    Gr_Logger().Get()<<"Load cont. Set BlockType win";
    //title
    Gr_Panel *pb1=new Gr_Panel(0,0,w_w,Get1cm()*1.5);
    pb1->left=true;
    pb1->top=true;
    pb1->LoadImg(":/images/other/pic/menu_bg.png");
    pb1->static_w=false;
    Windows[WI_BT]->Panels.push_back(pb1);
    //back btn
    Gr_Drawable *bt_back_btn=new Gr_Drawable(0,0,Get1cm()*1.5,Get1cm()*1.5,DRWABLE_BUTTON,DE_BT_BACK);
    bt_back_btn->LoadImg(":/images/toolbar/pic/back_btn.png");
    Windows[WI_BT]->Panels[0]->Objects.push_back(bt_back_btn);

    //content
    Gr_Panel *pb2=new Gr_Panel(0,Get1cm()*1.5,w_w,w_h-Get1cm()*1.5);
    pb2->left=true;
    pb2->top=true;
    pb2->resize_cont=true;
    pb2->static_h=false;
    pb2->static_w=false;
    Windows[WI_BT]->Panels.push_back(pb2);

    Gr_Drawable *c0=new Gr_Drawable(0,0,634,261,DRWABLE_BUTTON,DE_BT_COMMON);
    c0->LoadImg(":/images/toolbar/pic/tb_common.png");
    Windows[WI_BT]->Panels[1]->Objects.push_back(c0);

    Gr_Drawable *c1=new Gr_Drawable(0,0,634,261,DRWABLE_BUTTON,DE_BT_SOURCE);
    c1->LoadImg(":/images/toolbar/pic/tb_source.png");
    Windows[WI_BT]->Panels[1]->Objects.push_back(c1);

    Gr_Drawable *c2=new Gr_Drawable(0,0,634,261,DRWABLE_BUTTON,DE_BT_MATH);
    c2->LoadImg(":/images/toolbar/pic/tb_arithmetic.png");
    Windows[WI_BT]->Panels[1]->Objects.push_back(c2);

    Gr_Drawable *c3=new Gr_Drawable(0,0,634,261,DRWABLE_BUTTON,DE_BT_LINE);
    c3->LoadImg(":/images/toolbar/pic/tb_linear.png");
    Windows[WI_BT]->Panels[1]->Objects.push_back(c3);

    Gr_Drawable *c4=new Gr_Drawable(0,0,634,261,DRWABLE_BUTTON,DE_BT_SCOPE);
    c4->LoadImg(":/images/toolbar/pic/tb_scope.png");
    Windows[WI_BT]->Panels[1]->Objects.push_back(c4);

    Gr_Drawable *c5=new Gr_Drawable(0,0,634,261,DRWABLE_BUTTON,DE_BT_NONLINE);
    c5->LoadImg(":/images/toolbar/pic/tb_nonlinear.png");
    Windows[WI_BT]->Panels[1]->Objects.push_back(c5);

    Gr_Drawable *c6=new Gr_Drawable(0,0,634,261,DRWABLE_BUTTON,DE_BT_LOGIC);
    c6->LoadImg(":/images/toolbar/pic/tb_logic.png");
    Windows[WI_BT]->Panels[1]->Objects.push_back(c6);

    //COMMON BLOCK ADD
    Gr_Logger().Get()<<"Load cont. Set AddBlock win";
    //title
    Gr_Panel *pa1=new Gr_Panel(0,0,w_w,Get1cm()*1.5);
    pa1->left=true;
    pa1->top=true;
    pa1->LoadImg(":/images/other/pic/menu_bg.png");
    pa1->static_w=false;
    Windows[WI_AB]->Panels.push_back(pa1);
    //back btn
    Gr_Drawable *ab_back_btn=new Gr_Drawable(0,0,Get1cm()*1.5,Get1cm()*1.5,DRWABLE_BUTTON,DE_AB_BACK);
    ab_back_btn->LoadImg(":/images/toolbar/pic/back_btn.png");
    Windows[WI_AB]->Panels[0]->Objects.push_back(ab_back_btn);

    //content
    Gr_Panel *p_fav=new Gr_Panel(0,Get1cm()*1.5,w_w,w_h-Get1cm()*1.5);
    p_fav->SetScalable();

    //FAV PAN
    core_obj_types ct;
    int n_in,n_out;
    Gr_DrawCore *o=NULL;
    //... fav bloks
    vector<core_obj_types> fav_ind={COT_SOURCE,COT_SCP,COT_DISPLAY,COT_ADD,COT_SUB,COT_MULT,COT_DIV,COT_GAIN,COT_DELAY,COT_SQRT,COT_SQR,COT_SIN,COT_TAN,COT_ATAN2,COT_INTEG,COT_TRANSFER};
    for(int i=0;i<fav_ind.size();i++)
    {
        ct=fav_ind[i];
        o=new Gr_DrawCore(0,0,Gr_Core::NumOfInputs(ct),Gr_Core::NumOfOutputs(ct),DRWABLE_TOOLS,ct);
        o->isStatic=true;
        LoadImg(o);
        p_fav->Objects.push_back(o);
    }
    Windows[WI_AB]->Panels.push_back(p_fav);

    //SOURCE PAN
    Gr_Panel *p_source=new Gr_Panel(0,Get1cm()*1.5,w_w,w_h-Get1cm()*1.5);
    p_source->SetScalable();
    for(int i=COT_SOURCE;i<=COT_TIME;i++)
    {
        ct=(core_obj_types)i;
        o=new Gr_DrawCore(0,0,Gr_Core::NumOfInputs(ct),Gr_Core::NumOfOutputs(ct),DRWABLE_TOOLS,ct);
        o->isStatic=true;
        LoadImg(o);
        p_source->Objects.push_back(o);
    }
    Windows[WI_AB]->Panels.push_back(p_source);

    //SCOPE PAN
    Gr_Panel *p_scp=new Gr_Panel(0,Get1cm()*1.5,w_w,w_h-Get1cm()*1.5);
    p_scp->SetScalable();
    for(int i=COT_SCP;i<=COT_SCP_4PLOTS;i++)
    {
        ct=(core_obj_types)i;
        o=new Gr_DrawCore(0,0,Gr_Core::NumOfInputs(ct),Gr_Core::NumOfOutputs(ct),DRWABLE_TOOLS,ct);
        o->isStatic=true;
        LoadImg(o);
        p_scp->Objects.push_back(o);
    }
    Windows[WI_AB]->Panels.push_back(p_scp);

    //LOGIC PAN
    Gr_Panel *p_log=new Gr_Panel(0,Get1cm()*1.5,w_w,w_h-Get1cm()*1.5);
    p_log->SetScalable();
    for(int i=COT_S0;i<=COT_RSTRIGGER;i++)
    {
        ct=(core_obj_types)i;
        o=new Gr_DrawCore(0,0,Gr_Core::NumOfInputs(ct),Gr_Core::NumOfOutputs(ct),DRWABLE_TOOLS,ct);
        o->isStatic=true;
        LoadImg(o);
        p_log->Objects.push_back(o);
    }
    Windows[WI_AB]->Panels.push_back(p_log);

    //MATH PAN
    Gr_Panel *p_math=new Gr_Panel(0,Get1cm()*1.5,w_w,w_h-Get1cm()*1.5);
    p_math->SetScalable();
    for(int i=COT_ADD;i<=COT_LN;i++)
    {
        ct=(core_obj_types)i;
        o=new Gr_DrawCore(0,0,Gr_Core::NumOfInputs(ct),Gr_Core::NumOfOutputs(ct),DRWABLE_TOOLS,ct);
        o->isStatic=true;
        LoadImg(o);
        p_math->Objects.push_back(o);
    }
    Windows[WI_AB]->Panels.push_back(p_math);

    //LINEAR PAN
    Gr_Panel *p_line=new Gr_Panel(0,Get1cm()*1.5,w_w,w_h-Get1cm()*1.5);
    p_line->SetScalable();
    for(int i=COT_INTEG;i<=COT_HARMONIC;i++)
    {
        ct=(core_obj_types)i;
        o=new Gr_DrawCore(0,0,Gr_Core::NumOfInputs(ct),Gr_Core::NumOfOutputs(ct),DRWABLE_TOOLS,ct);
        o->isStatic=true;
        LoadImg(o);
        p_line->Objects.push_back(o);
    }
    Windows[WI_AB]->Panels.push_back(p_line);

    //NON-LINEAR PAN
    Gr_Panel *p_nonline=new Gr_Panel(0,Get1cm()*1.5,w_w,w_h-Get1cm()*1.5);
    p_nonline->SetScalable();
    for(int i=COT_BACKLASH;i<=COT_DEADZONE;i++)
    {
        ct=(core_obj_types)i;
        o=new Gr_DrawCore(0,0,Gr_Core::NumOfInputs(ct),Gr_Core::NumOfOutputs(ct),DRWABLE_TOOLS,ct);
        o->isStatic=true;
        LoadImg(o);
        p_nonline->Objects.push_back(o);
    }
    Windows[WI_AB]->Panels.push_back(p_nonline);

    //MAIN WIN
    Gr_Logger().Get()<<"Load cont. Set Main win";
    Gr_Panel *p_main_btns=new Gr_Panel(0,0,Get1cm()*8,Get1cm()*8);
    p_main_btns->mid_vert=true;
    p_main_btns->mid_horiz=true;
    Gr_Drawable *main_btn1=new Gr_Drawable(Get1cm()*0.5,Get1cm()*0.5,7*Get1cm(),2*Get1cm(),DRWABLE_STATIC,DE_MW_NEW);
    main_btn1->LoadImg(":/images/toolbar/pic/btn_newfile.png");
    Gr_Drawable *main_btn2=new Gr_Drawable(Get1cm()*0.5,Get1cm()*3,7*Get1cm(),2*Get1cm(),DRWABLE_STATIC,DE_MW_LOAD);
    main_btn2->LoadImg(":/images/toolbar/pic/btn_loadfile.png");
    Gr_Drawable *main_btn3=new Gr_Drawable(Get1cm()*0.5,Get1cm()*5.5,7*Get1cm(),2*Get1cm(),DRWABLE_STATIC,DE_MW_REPORT);
    main_btn3->LoadImg(":/images/toolbar/pic/btn_report.png");
    main_btn1->DrawBorder=true;
    main_btn2->DrawBorder=true;
    main_btn3->DrawBorder=true;
    p_main_btns->DrawBorder=true;
    p_main_btns->Objects.push_back(main_btn1);
    p_main_btns->Objects.push_back(main_btn2);
    p_main_btns->Objects.push_back(main_btn3);
    p_main_btns->DrawBorder=false;
    Windows[WI_MAIN]->LoadImg(":/images/other/pic/main_menu.png");
    Windows[WI_MAIN]->Panels.push_back(p_main_btns);

    //GRAPH WIN
    Gr_Logger().Get()<<"Load cont. Set Graph win";
    Windows[WI_GR]->Panels.push_back(p2);

    Gr_Logger().Get()<<"Content loaded";
}

//convert from interface to core
void MainWindow::Convert()
{
    Gr_Logger().Get()<<"Convert from Interface to Core";
    //create new core object
    delete core;
    core=new Gr_Core;
    Gr_Logger().Get()<<"Convert objects";
    //for each workspace element create core-object with core-index
    for(int i=0;i<Drawables.size();i++)
    {
        Gr_Logger().Get()<<"==============================="<<endl
                        <<"Convert object:"<<endl
                        <<"ind: "<<i<<"; type: "<<Drawables[i]->core_type<<endl
                        <<"InpN: "<<Drawables[i]->inpN<<"; OutpN: "<<Drawables[i]->outpN;

        QString str("");
        for(int j=0;j<Drawables[i]->pars.size();j++)
        {
            str+=QString::number(Drawables[i]->pars[j])+" ";
        }
        Gr_Logger().Get()<<"Par: "<<str.toStdString();

        str="";
        for(int j=0;j<Drawables[i]->states.size();j++)
        {
            str+=QString::number(Drawables[i]->states[j])+" ";
        }
        Gr_Logger().Get()<<"States: "<<str.toStdString();

        if(Drawables[i]->GetType()==DRWABLE_WORKSPACE)
            core->Add(Drawables[i]->core_type,Drawables[i]->inpN,Drawables[i]->outpN,Drawables[i]->pars,Drawables[i]->states,&Drawables[i]->data);
    }
    Gr_Logger().Get()<<"===============================";
    Gr_Logger().Get()<<"Link objects";
    //transform lines information into core-links
    for(int i=0;i<Lines.size();i++)
    {
        Gr_Logger().Get()<<"Link: "<<Lines[i]->in_obj->calcInd<<"-"<<Lines[i]->numIn<<" & "<<Lines[i]->out_obj->calcInd<<"-"<<Lines[i]->numOut;
        core->Link(Lines[i]->in_obj->calcInd,Lines[i]->numIn,Lines[i]->out_obj->calcInd,Lines[i]->numOut);
    }
    Gr_Logger().Get()<<"===============================";
}

//calculate core
void MainWindow::Calc()
{
    Gr_Logger().Get()<<"Calc";
    //calc thread
    core->moveToThread(&CoreThread);
    connect(this, SIGNAL(calc_sign(double)), core, SLOT(Calc(double)));
    connect(core, SIGNAL(calc_fin()), this, SLOT(StopThread()));
    Gr_Logger().Get()<<"Start calc thread";
    CoreThread.start();

    if(core!=NULL)
    {
        Gr_Logger().Get()<<"Set sim step: "<<Ssim;
        //set sim step
        core->SetStep(Ssim);
        Gr_Logger().Get()<<"Set emit calc_signal";
        //start calc in new thread
        emit calc_sign(Tsim);
        //redraw graph
        //Graph->Prepare();
    }
    else
    {
        Gr_Logger().Get(LogError)<<"Can't Calc, no core object";
    }
}

int MainWindow::GetCalcInd()
{
    Gr_Logger().Get()<<"Get a new CalcInd";
    //first return and than increment calcInd
    return calcInd++;
}

bool MainWindow::GetSimData()
{
    Gr_Logger().Get()<<"Get SimData";
    //creatin a form
    QDialog dialog(this);
    QFormLayout form(&dialog);
    QList<QLineEdit *> fields_par;
    QLineEdit *lineEdit1 = new QLineEdit(&dialog);
    lineEdit1->setInputMethodHints(Qt::ImhPreferNumbers);
    lineEdit1->setValidator( new QDoubleValidator() );
    lineEdit1->setText(QString::number(Tsim));

    QString label1 = QString("Sim Time");
    form.addRow(label1, lineEdit1);
    fields_par << lineEdit1;

    QLineEdit *lineEdit2 = new QLineEdit(&dialog);
    lineEdit2->setInputMethodHints(Qt::ImhPreferNumbers);
    lineEdit2->setValidator( new QDoubleValidator() );
    lineEdit2->setText(QString::number(Ssim));
    QString label2 = QString("Sim Step");
    form.addRow(label2, lineEdit2);
    fields_par << lineEdit2;

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    dialog.show();
    //show keyboard on dial.show
    /*lineEdit1->setFocus();
    lineEdit1->selectAll();
    QInputMethod *keyboard = QGuiApplication::inputMethod();
    keyboard->show();*/

    //if Ok was clicked
    if (dialog.exec() == QDialog::Accepted)
    {
        Tsim=fields_par[0]->text().replace(',','.').toDouble();
        Ssim=fields_par[1]->text().replace(',','.').toDouble();
        return true;
    }
    return false;
}

bool MainWindow::Save(bool PopFlg)
{
    Gr_Logger().Get()<<"Save function; flag: "<<PopFlg;
    if(PopFlg)
    {
        QDialog dialog(this);
        QFormLayout form(&dialog);
        QList<QLineEdit *> fields_par;
        QLineEdit *lineEdit1 = new QLineEdit(&dialog);
        lineEdit1->setInputMethodHints(Qt::ImhPreferLatin);
        lineEdit1->setText(QString("New_File"));

        QString label1 = QString("File Name:");
        form.addRow(label1, lineEdit1);
        fields_par << lineEdit1;

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                   Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));


        //show keyboard on dial.show
        dialog.show();
        lineEdit1->setFocus();
        lineEdit1->selectAll();
        QInputMethod *keyboard = QGuiApplication::inputMethod();
        keyboard->show();

        //if Ok was clicked

        QString f_n;
        if (dialog.exec() == QDialog::Accepted)
        {
            f_n=lineEdit1->text();
            f_n+=".data";
            QFile f;
            f.setFileName(AppDir+f_n);
            if(f.exists())
            {
                QDialog DiaRewrite(this);
                QFormLayout form2(&DiaRewrite);
                QLabel label2("This file is already exist.\nOverwrite it?");
                form2.addRow(&label2);

                // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
                QDialogButtonBox buttonBox2(QDialogButtonBox::Yes | QDialogButtonBox::Cancel,
                                           Qt::Horizontal, &DiaRewrite);
                form2.addRow(&buttonBox2);
                QObject::connect(&buttonBox2, SIGNAL(accepted()), &DiaRewrite, SLOT(accept()));
                QObject::connect(&buttonBox2, SIGNAL(rejected()), &DiaRewrite, SLOT(reject()));

                if (DiaRewrite.exec() != QDialog::Accepted)
                    return false;
            }

            //clear WS
            Clear();

        }
        else
            return false;

        file_name=f_n;
    }

    QFile qfile(AppDir+file_name);
    if (!qfile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&qfile);

    //ofstream file(dir_file.toLatin1().data(),std::ofstream::trunc);
    //if(!file) return false;

    for(int i=0;i<Drawables.size();i++)
    {
        Gr_DrawCore *o=Drawables[i];
        //don't save deleted items
        if(!o->isDrawn) continue;
        out << o->core_type << " " << o->calcInd << " " << o->x << " " << o->y << " " << o->isRotated << endl;
        for(int i=0;i<o->pars.size();i++)
        {
            out << o->pars[i];
            if(i<o->pars.size()-1)
                out << " ";
        }
        out << endl;
        for(int i=0;i<o->states.size();i++)
        {
            out << o->states[i];
            if(i<o->states.size()-1)
                out << " ";
        }
        out << endl;
    }
    out << "%" << endl;
    for(int i=0;i<Lines.size();i++)
    {
        Gr_Line *l=Lines[i];
        out << l->in_obj->calcInd << " " << l->numIn << " "
            << l->out_obj->calcInd << " " << l->numOut << endl;
    }
    out << "%" << endl;
    out<<grid_step_smooth<<" "<<grid_offset_smoth_X<<" "<<grid_offset_smoth_Y<<endl;
    out<<Tsim<<" "<<Ssim;
    qfile.close();
    return true;
}

bool MainWindow::Load()
{
    Gr_Logger().Get()<<"Load function";
    QString f_n;
    QFileDialog dial(this);
    dial.setOptions(QFileDialog::HideNameFilterDetails);
    dial.setViewMode(QFileDialog::List);
    dial.setDirectory(AppDir);
    dial.showFullScreen();
    if(dial.exec())
    {
        if(dial.selectedFiles().size()==1)
            f_n=dial.selectedFiles()[0];
    }
    else
        return false;

    dir_file=f_n;

    //clear WS
    Clear();

    char read_buffer[MAX_STR_SIZE];
    QString buff;
    QTextStream scan;

    QFile qfile(dir_file);
    if (!qfile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QFileInfo fileInfo(qfile.fileName());
    file_name=fileInfo.fileName();

    QTextStream in(&qfile);

    while(!in.atEnd())
    {
        int c_type;
        int cId, xx, yy, rot;
        string str_pars,str_st;
        int n_par,n_st;
        Gr_DrawCore *o;


        buff=in.readLine();
        scan.setString(&buff);
        scan.seek(0);
        if(buff==QString("%")) break;

        scan>>c_type>>cId>>xx>>yy>>rot;

        n_par=Gr_Core::NumOfPars((core_obj_types)c_type);
        n_st=Gr_Core::NumOfStates((core_obj_types)c_type);
        o=AddBlock((core_obj_types)c_type);
        o->x=xx;
        o->y=yy;
        o->isRotated=rot;
        o->calcInd=cId;
        if(cId>calcInd) calcInd=cId+1;

        buff=in.readLine();;
        scan.setString(&buff);
        scan.seek(0);
        for(int i=0;i<n_par;i++)
        {
            float par;
            if(!scan.atEnd())
            {
                scan>>par;
                o->pars[i]=par;
            }
        }
        buff=in.readLine();
        scan.setString(&buff);
        scan.seek(0);
        for(int i=0;i<n_st;i++)
        {
            float st;
            if(!scan.atEnd())
            {
                scan>>st;
                o->states[i]=st;
            }
        }
    }
    while(!in.atEnd())
    {
        buff=in.readLine();
        scan.setString(&buff);
        scan.seek(0);
        if(buff==QString("%")) break;
        int o_in,n_in,o_out,n_out;
        scan>>o_in>>n_in>>o_out>>n_out;

        Gr_Line *l=new Gr_Line();
        Gr_DrawCore *obj_in,*obj_out;
        bool flg_in=false,flg_out=false;
        for(int i=0;i<Drawables.size();i++)
        {
            if(Drawables[i]->calcInd==o_in)
            {
                obj_in=Drawables[i];
                flg_in=true;
            }
            if(Drawables[i]->calcInd==o_out)
            {
                obj_out=Drawables[i];
                flg_out=true;
            }
            if(flg_out && flg_in)
            {
                l->SetPi(obj_in,n_in);
                l->SetPo(obj_out,n_out);
                Lines.push_back(l);
                break;
            }
        }

    }

    buff=in.readLine();
    scan.setString(&buff);
    scan.seek(0);
    scan>>grid_step_smooth>>grid_offset_smoth_X>>grid_offset_smoth_Y;
    UpdGrid();

    buff=in.readLine();
    scan.setString(&buff);
    scan.seek(0);
    scan>>Tsim>>Ssim;
    qfile.close();
    return true;
}

Gr_DrawCore* MainWindow::AddBlock(core_obj_types ct)
{
    Gr_Logger().Get()<<"AddBlock by it's core_obj_types, type: "<<(int)ct;
    Gr_DrawCore *obj=NULL;

    int pos_x=w_w/2+0.5;
    int pos_y=w_h/2+0.5;

    int n_in=Gr_Core::NumOfInputs(ct), n_out=Gr_Core::NumOfOutputs(ct);

    obj=new Gr_DrawCore(pos_x,pos_y,n_in,n_out,DRWABLE_WORKSPACE,ct);

    vector<double> p=Gr_Core::GetInitCond(ct);
    for(int i=0;i<Gr_Core::NumOfPars(ct);i++)
    {
        obj->pars[i]=p[i];
    }

    for(int i=0;i<Gr_Core::NumOfStates(ct);i++)
    {
        obj->states[i]=0;
    }

    LoadImg(obj);

    if(obj!=NULL)
    {
        obj->calcInd=GetCalcInd();
        Drawables.push_back(obj);
    }

    return obj;
}

void MainWindow::LoadImg(Gr_DrawCore *obj)
{
    Gr_Logger().Get()<<"Load Img for an object by it's core_type, type: "<<obj->core_type;
    switch (obj->core_type)
    {
    case COT_SOURCE:
        obj->LoadImg(":/images/elements/pic/el_const.png");
        break;
    case COT_ADD:
    case COT_ADD3:
    case COT_ADD4:
        obj->LoadImg(":/images/elements/pic/el_sum.png");
        break;
    case COT_GAIN:
        obj->LoadImg(":/images/elements/pic/el_gain.png");
        break;
    case COT_INTEG:
        obj->LoadImg(":/images/elements/pic/el_integrator.png");
        break;
    case COT_INTEG2:
        obj->LoadImg(":/images/elements/pic/el_integrator2.png");
        break;
    case COT_SCP:
    case COT_SCP_XY:
        obj->LoadImg(":/images/elements/pic/el_scope.png");
        break;
    case COT_SCP_4PLOTS:
        obj->LoadImg(":/images/elements/pic/el_scope4.png");
        break;
    case COT_DELAY:
        obj->LoadImg(":/images/elements/pic/el_delay.png");
        break;
    case COT_AND:
        obj->LoadImg(":/images/elements/pic/el_and.png");
        break;
    case COT_OR:
        obj->LoadImg(":/images/elements/pic/el_or.png");
        break;
    case COT_XOR:
        obj->LoadImg(":/images/elements/pic/el_xor.png");
        break;
    case COT_NOT:
        obj->LoadImg(":/images/elements/pic/el_not.png");
        break;
    case COT_DIFF:
        obj->LoadImg(":/images/elements/pic/el_diff.png");
        break;
    case COT_PULSE:
        obj->LoadImg(":/images/elements/pic/el_puls.png");
        break;
    case COT_SIN_SOURCE:
        obj->LoadImg(":/images/elements/pic/el_sin_source.png");
        break;
    case COT_STEP:
        obj->LoadImg(":/images/elements/pic/el_step.png");
        break;
    case COT_TIME:
        obj->LoadImg(":/images/elements/pic/el_time.png");
        break;
    case COT_S0:
        obj->LoadImg(":/images/elements/pic/el_s0.png");
        break;
    case COT_S1:
        obj->LoadImg(":/images/elements/pic/el_s1.png");
        break;
    case COT_DISPLAY:
        break;
    case COT_SUB:
        obj->LoadImg(":/images/elements/pic/el_sub.png");
        break;
    case COT_DIV:
        obj->LoadImg(":/images/elements/pic/el_div.png");
        break;
    case COT_MULT:
    case COT_MULT3:
        obj->LoadImg(":/images/elements/pic/el_mul.png");
        break;
    case COT_SIGN:
        obj->LoadImg(":/images/elements/pic/el_sign.png");
        break;
    case COT_ABS:
        obj->LoadImg(":/images/elements/pic/el_abs.png");
        break;
    case COT_SQRT:
        obj->LoadImg(":/images/elements/pic/el_sqrt.png");
        break;
    case COT_SQR:
        obj->LoadImg(":/images/elements/pic/el_sqr.png");
        break;
    case COT_POW:
        obj->LoadImg(":/images/elements/pic/el_pow.png");
        break;
    case COT_SIN:
        obj->LoadImg(":/images/elements/pic/el_sin.png");
        break;
    case COT_COS:
        obj->LoadImg(":/images/elements/pic/el_cos.png");
        break;
    case COT_TAN:
        obj->LoadImg(":/images/elements/pic/el_tan.png");
        break;
    case COT_ASIN:
        obj->LoadImg(":/images/elements/pic/el_asin.png");
        break;
    case COT_ACOS:
        obj->LoadImg(":/images/elements/pic/el_acos.png");
        break;
    case COT_ATAN:
    case COT_ATAN2:
        obj->LoadImg(":/images/elements/pic/el_atan.png");
        break;
    case COT_EXP:
        obj->LoadImg(":/images/elements/pic/el_exp.png");
        break;
    case COT_LOG10:
        obj->LoadImg(":/images/elements/pic/el_log.png");
        break;
    case COT_LN:
        obj->LoadImg(":/images/elements/pic/el_ln.png");
        break;
    case COT_TRANSFER:
        obj->LoadImg(":/images/elements/pic/el_transf.png");
        break;
    case COT_HARMONIC:
        obj->LoadImg(":/images/elements/pic/el_harmonic.png");
        break;
    case COT_BACKLASH:
        obj->LoadImg(":/images/elements/pic/el_backlash.png");
        break;
    case COT_SATURATION:
        obj->LoadImg(":/images/elements/pic/el_satur.png");
        break;
    case COT_HYSTER:
        obj->LoadImg(":/images/elements/pic/el_hyster.png");
        break;
    case COT_DEADZONE:
        obj->LoadImg(":/images/elements/pic/el_deadzone.png");
        break;
    case COT_COMPARE:
        obj->LoadImg(":/images/elements/pic/el_compare.png");
        break;
    case COT_SWITCH:
        obj->LoadImg(":/images/elements/pic/el_switch.png");
        break;
    case COT_RSTRIGGER:
        obj->LoadImg(":/images/elements/pic/el_RStrig.png");
        break;
    default:
        obj->LoadImg(":/images/elements/pic/no_image.png");
        break;
    }

}

void MainWindow::UpdGrid()
{
    GRID_STEP=(int)grid_step_smooth;
    GRID_OFFSET_X=(int)(grid_offset_smoth_X);
    GRID_OFFSET_Y=(int)(grid_offset_smoth_Y);
}

void MainWindow::Clear()
{
    Gr_Logger().Get()<<"Clear workspace function";
    Drawables.clear();
    Lines.clear();
    grid_step_smooth=GRID_STEP0;
    grid_offset_smoth_X=0;
    grid_offset_smoth_Y=0;
    UpdGrid();
    calcInd=0;
}

void MainWindow::StartCalc()
{
    Gr_Logger().Get()<<"Start Calc fun";
    if(CoreThread.isRunning())
    {
        Gr_Logger().Get(LogError)<<"Calc thread is still active";
        return;
    }

    if(GetSimData())
    {
        Gr_Logger().Get()<<"Starting calc";
        Save();
        Convert();
        Calc();
    }
}

void MainWindow::PrepareGra()
{
    Graph->Prepare();
}

void MainWindow::ClearHL()
{
    Gr_Logger().Get()<<"Clear HL fun";
    for(int i=0; i<Drawables.size();i++)
    {
        Drawables[i]->wasClicked=false;
    }
    for(int i=0;i<Lines.size();i++)
    {
        Lines[i]->isHL=false;
    }
    ClearPutsHL();
}

void MainWindow::DeleteObjs()
{
    Gr_Logger().Get()<<"Delete Objects fun";
    QDialog DiaDel(this);
    QFormLayout form2(&DiaDel);
    QLabel label2("Do you want to delete selected files?");
    form2.addRow(&label2);

    QDialogButtonBox buttonBox2(QDialogButtonBox::Yes | QDialogButtonBox::No,
                               Qt::Horizontal, &DiaDel);
    form2.addRow(&buttonBox2);
    QObject::connect(&buttonBox2, SIGNAL(accepted()), &DiaDel, SLOT(accept()));
    QObject::connect(&buttonBox2, SIGNAL(rejected()), &DiaDel, SLOT(reject()));

    if (DiaDel.exec() != QDialog::Accepted)
        return;

    for(int i=Drawables.size()-1;i>=0;i--)
    {
        if(Drawables[i]->wasClicked)
        {
            for(int j=Lines.size()-1;j>=0;j--)
            {
                if(Lines[j]->in_obj==Drawables[i] || Lines[j]->out_obj==Drawables[i])
                {
                    Lines.erase(Lines.begin()+j);
                }
            }
            //correct calcIndexes
            Drawables.erase(Drawables.begin()+i);
            for(int j=i;j<Drawables.size();j++)
            {
                Drawables[j]->calcInd--;
            }
            calcInd--;
        }
    }
    for(int i=Lines.size()-1;i>=0;i--)
    {
        if(Lines[i]->isHL)
            Lines.erase(Lines.begin()+i);
    }
    DotInExist=false;
    DotOutExist=false;
}

void MainWindow::ToggleHL(int ind)
{
    Gr_Logger().Get()<<"Toggle object HL";
    Drawables[ind]->wasClicked=!Drawables[ind]->wasClicked;
}

void MainWindow::ClearPutsHL()
{
    for(int i=0;i<Drawables.size();i++)
    {
        Drawables[i]->putWasClicked=0;
        Drawables[i]->clickedPutInd=0;
    }
}

void MainWindow::SetPutsHL(int ind, int in_out,int num)
{
    if(ind<0 || ind>=Drawables.size())
    {
        Gr_Logger().Get(LogError)<<"Incorrect HL index";
        return;
    }
    Drawables[ind]->SetHL(in_out,num);
}

void MainWindow::ChangeWindow(window_index w)
{
    Gr_Logger().Get()<<"Win is chaneged to: "<<WinToString(w);
    ActiveWin=w;
}

void MainWindow::SetIsHold(bool flg)
{
    Gr_Logger().Get()<<"IsHold is changed to: "<<flg;
    isHold=flg;
}

void MainWindow::StopTapHoldTimer()
{
    Gr_Logger().Get()<<"Tap&Hold timer is stoped";
    tapHoldTimer->stop();
}

void MainWindow::SetIndPressed(int ind)
{
    Gr_Logger().Get()<<"Set Index Pressed: "<<ind;
    ind_pressed=ind;
}

void MainWindow::ResetIndPressed()
{
    Gr_Logger().Get()<<"Reset Index Pressed";
    ind_pressed=-1;
}

bool MainWindow::DoesIndExist(int ind)
{
    return ind>=0&&ind<calcInd?true:false;
}

void MainWindow::AppStateEv(Qt::ApplicationState state)
{
    if(state==Qt::ApplicationState::ApplicationSuspended)
    {
        Gr_Logger().Get()<<"App is suspended";
        Save();
    }
}

void MainWindow::AboutToQuit()
{
    Gr_Logger().Get()<<"App is About to quit";
    Save();
}

void MainWindow::StopThread()
{
    Gr_Logger().Get()<<"Stop calc thread";
    graphUpd->stop();
    CoreThread.quit();
    CoreThread.wait();
    Graph->Prepare();
}

void MainWindow::ResetFlags()
{
    Gr_Logger().Get()<<"Reset click flags";
    //stop tap&hold timer
    StopTapHoldTimer();
    //reset click
    clk.first=-1;
    clk.second=-1;
    //stop scrolling
    scroll=false;
    may_scroll=false;
    //stop dragging
    isDrag=0;
    drag_ind=-1;
    //stop offsetting grid
    isOffset=false;
    //press index
    ResetIndPressed();
    //press and hold
    SetIsHold(false);
}

bool MainWindow::isBodyClick(int ind)
{
    return (Drawables[ind]->InputClicked(mX,mY))==-1 && (Drawables[ind]->OutputClicked(mX,mY))==-1;
}

void MainWindow::ResetDblClk()
{
    Gr_Logger().Get()<<"Reset double clk flags";
    doubleClkTimer->stop();
    DoubleClkFlg=false;
    DblRotate=false;
    RotateInd=-1;
}


bool MainWindow::SetSAndP(Gr_DrawCore* o)
{
    Gr_Logger().Get()<<"Set Parameters & States fun";
    int n_st,n_par;
    n_st=Gr_Core::NumOfStates(o->core_type);
    n_par=Gr_Core::NumOfPars(o->core_type);
    vector<string> p_name=Gr_Core::GetParNames(o->core_type);

    if(o->pars.size()!=n_par || o->states.size()!=n_st)
    {
        Gr_Logger().Get(LogError)<<"Can't set params or states";
        return false;
    }

    if(n_st>0 || n_par>0)
    {
        Gr_Logger().Get()<<"Setting pars & states";
        //vibrate
        #if defined(Q_OS_ANDROID)
        Gr_Logger().Get()<<"Vibrate";
        vibrator.vibrate(VIBRATE_TIME);
        #endif

        QDialog dialog(this);
        dialog.setInputMethodHints(Qt::ImhPreferNumbers);
        // Use a layout allowing to have a label next to each field
        QFormLayout form(&dialog);

        QList<QLineEdit *> fields_par;
        if(n_par>0)
        {
            // Add text above the fields
            form.addRow(new QLabel("Parameters:"));
            // Add the lineEdits with their respective labels
            for(int i = 0; i < p_name.size(); ++i)
            {
                QLineEdit *lineEdit = new QLineEdit(&dialog);
                lineEdit->setInputMethodHints(Qt::ImhPreferNumbers);
                lineEdit->setValidator( new QDoubleValidator() );
                lineEdit->setText(QString::number(o->pars[i]));
                lineEdit->selectAll();
                QString label = p_name[i].c_str();
                form.addRow(label, lineEdit);

                fields_par << lineEdit;
            }
        }

        QList<QLineEdit *> fields_states;
        if(n_st>0)
        {
            // Add some text above the fields
            form.addRow(new QLabel("States:"));
            for(int i = 0; i < n_st; ++i)
            {
                QLineEdit *lineEdit = new QLineEdit(&dialog);
                lineEdit->setInputMethodHints(Qt::ImhPreferNumbers);
                lineEdit->setValidator( new QDoubleValidator() );
                lineEdit->setText(QString::number(o->states[i]));
                lineEdit->selectAll();

                QString label=QString("State %1").arg(i + 1);

                form.addRow(label, lineEdit);
                fields_states << lineEdit;
            }
        }

        // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                   Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));


        dialog.show();

        //show keyboard
        /*if(fields_par.size()>0)
        {
            fields_par[0]->setFocus();
            fields_par[0]->selectAll();
        }
        else  if(fields_states.size()>0)
        {
            fields_states[0]->setFocus();
            fields_states[0]->selectAll();
        }
        QInputMethod *keyboard = QGuiApplication::inputMethod();
        keyboard->show();*/

        // Show the dialog as modal
        if (dialog.exec() == QDialog::Accepted)
        {
            // If the user didn't dismiss the dialog, do something with the fields
            for(int i=0;i<fields_par.size();i++)
            {
                double val=fields_par[i]->text().replace(',','.').toDouble();
                o->pars[i]=val;
            }
            for(int i=0;i<fields_states.size();i++)
            {
                double val=fields_states[i]->text().replace(',','.').toDouble();
                o->states[i]=val;
            }
        }
        return  true;
    }
    return false;
}



