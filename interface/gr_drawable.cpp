#include "gr_drawable.h"

Gr_Drawable::Gr_Drawable()
{
    class_type=DC_DRAWABLE;
}

Gr_Drawable::Gr_Drawable(int xx, int yy, int ww, int hh,drawable_types tt,drawable_events ev)
{
    class_type=DC_DRAWABLE;

    x=xx;
    x0=x;

    y=yy;
    y0=y;

    width=ww;
    w0=width;

    height=hh;
    h0=height;

    type=tt;
    event=ev;
}

void Gr_Drawable::Draw(QPainter *painter, QPaintEvent *event, int dx, int dy, int sx, int sy)
{
    if(!isDrawn)
        return;

    //draw image
    if(ImgFlag)
    {
        if(imgDraw->size().width()!=(int)(sx*width) && imgDraw->size().height()!=(int)(sy*height))
            *imgDraw=img->scaled((int)(sx*width),(int)(sy*height));
        painter->drawPixmap(dx+x,dy+y,*imgDraw);
    }
    //draw borders
    if(DrawBorder)
    {
        QRectF Reg(dx+x,dy+y,sx*width,sy*height);
        painter->drawRect(Reg);
    }
}

bool Gr_Drawable::Clicked(int xx, int yy)
{
    if(!isDrawn) return false;

    if(xx>x && xx<x+width)
    {
        if(yy>y && yy<y+height)
        {
            return true;
        }
    }
    return false;
}

QPixmap* Gr_Drawable::GetImg()
{
    QPixmap* tempImg=new QPixmap(*img);
    return tempImg;
}

void Gr_Drawable::SetImg(QPixmap* timg)
{
    img=timg;
    imgDraw=new QPixmap(*img);
    ImgFlag=true;
}

void Gr_Drawable::Show(bool flg)
{
    isDrawn=flg;
}

void Gr_Drawable::LoadImg(const QString str)
{
    QImage img_t(str);
    //#if defined(Q_OS_ANDROID)
    //img_t=img_t.rgbSwapped();
    //#endif

    //converting image to ARGB
    //img_t=img_t.convertToFormat(QImage::Format::Format_A2BGR30_Premultiplied);
    //img_t=img_t.convertToFormat(QImage::Format::Format_ARGB32_Premultiplied);

    //scale image to reduce its size
    /*if(h0>DRWBLE_HEIGHT)
        img_t=img_t.scaled(w0*(DRWBLE_HEIGHT/h0),DRWBLE_HEIGHT);*/

    img=new QPixmap(img_t.size());

    //white bg is drawn faster than transperent
    if(img_t.hasAlphaChannel())
        img->fill(Qt::transparent);
    else
        img->fill(Qt::white);

    QPainter p(img);
    p.drawImage(0,0,img_t);
    p.end();

    imgDraw=new QPixmap(*img);
    ImgFlag=true;
}

drawable_types Gr_Drawable::GetType(){return type;}

void Gr_Drawable::SetType(drawable_types a){type=a;}

bool Gr_Drawable::HasImg(){return ImgFlag;}

int Gr_Drawable::GetX() {return x;}

int Gr_Drawable::GetY() {return y;}

int Gr_Drawable::GetWidth() {return width;}

int Gr_Drawable::GetHeight() {return height;}

void Gr_Drawable::SetX(int a) {x=a;}

void Gr_Drawable::SetY(int a) {y=a;}

void Gr_Drawable::SetSize(int ww,int hh)
{
    width=ww;
    height=hh;
}

void Gr_Drawable::SetPos(int xx,int yy)
{
    x=xx;
    y=yy;
}

//=========================================================================================================================
//DRAW_CORE

//void Gr_Drawable::SetNi(int a){inpN=a;}

//void Gr_Drawable::SetNo(int a){outpN=a;}

Gr_DrawCore::Gr_DrawCore(int xx, int yy, int Ni, int No, drawable_types tt,core_obj_types ct, bool stflg,drawable_events ev)
{
    class_type=DC_DRAWCORE;

    inpN=Ni;
    outpN=No;
    isStatic=stflg;
    event=ev;
    core_type=ct;

    for(int i=0;i<Gr_Core::NumOfPars(ct);i++)
    {
        pars.push_back(1);
    }
    for(int i=0;i<Gr_Core::NumOfStates(ct);i++)
    {
        states.push_back(0);
    }
    for(int i=0;i<Gr_Core::NumOfDataObjs(ct);i++)
    {
        data.push_back(vector<pair<double,double>>());
    }
    //put it on the grid
    x=GridFit(xx-GRID_OFFSET_X);
    y=GridFit(yy-GRID_OFFSET_Y);
    if(core_type==COT_DISPLAY || core_type==COT_HARMONIC)
        width_img=4;
    else
        width_img=2;
    width=width_img+2;
    height=((Ni>No?Ni:No)+1);
    w0=width_img*GRID_STEP0;
    h0=height*GRID_STEP0;
    type=tt;
    img=new QPixmap(width_img,height);
    imgDraw=new QPixmap(img->size());
    img->fill(Qt::transparent);
    imgDraw->fill(Qt::transparent);

    LoadImg(":/images/elements/pic/elem_bg.png");
}

Gr_DrawCore::Gr_DrawCore(Gr_DrawCore* drawt)
{
    /*class_type=DC_DRAWCORE;

    x=drawt->x;
    y=drawt->y;
    width=drawt->width;
    height=drawt->height;
    w0=width_img*GRID_STEP0;
    h0=height*GRID_STEP0;
    width_img=drawt->width_img;
    type=drawt->GetType();
    inpN=drawt->GetNi();
    outpN=drawt->GetNo();
    core_type=drawt->core_type;

    for(int i=0;i<drawt->pars.size();i++)
    {
        double *p_t=new double(*drawt->pars[i]);
        pars.push_back(p_t);
    }

    for(int i=0;i<drawt->states.size();i++)
    {
        double *s_t=new double(*drawt->states[i]);
        states.push_back(s_t);
    }

    //copy image if it exist
    SetImg(drawt->GetImg());*/
}

void Gr_DrawCore::LoadImg(const QString str)
{
    if(ImgFlag==false)
    {
        img->load(str);
        *img=img->scaled(w0,h0);
    }
    else
    {
        QPixmap i_temp(str);
        i_temp=i_temp.scaled(img->size(),Qt::KeepAspectRatio);
        QPainter pp(img);
        pp.drawPixmap(0,0,i_temp);
        pp.end();
    }

    ImgFlag=true;
}

void Gr_DrawCore::ResetHL()
{
    wasClicked=0;
    putWasClicked=0;
}

void Gr_DrawCore::SetHL(int p,int ind)
{
    putWasClicked=p;
    clickedPutInd=ind;
}

void Gr_DrawCore::Draw(QPainter *painter, QPaintEvent *event,int xx, int yy)
{
    if(!isDrawn)
        return;

    QPen linePen;
    linePen.setWidth((int)(0.1*Get1cm()+0.5));
    QPen hlPen(linePen);
    hlPen.setColor(Qt::red);
    hlPen.setWidth(linePen.width()*2);

    panel_dx=xx;
    panel_dy=yy;

    int g_st=isStatic?panel_grid_step:GRID_STEP;
    int go_x=(isStatic?panel_grid_off_x:GRID_OFFSET_X)+panel_dx;
    int go_y=(isStatic?panel_grid_off_y:GRID_OFFSET_Y)+panel_dy;

    //draw inputs (with arrows) and outputs
    int sign=isRotated?-1:1;
    for(int i=0;i<inpN;i++)
    {
        QPoint pa0=GetInpCoor(i);
        if(putWasClicked==1 && i==clickedPutInd)
            painter->setPen(hlPen);
        else
            painter->setPen(linePen);

        painter->drawLine(pa0.x(),pa0.y(),pa0.x()+sign*g_st,pa0.y());
        pa0.rx()=pa0.x()+sign*g_st;
        QPoint pa1,pa2;
        pa1.rx()=pa0.x()-sign*g_st/2;
        pa1.ry()=pa0.y()-g_st/3;
        pa2.rx()=pa0.x()-sign*g_st/2;
        pa2.ry()=pa0.y()+g_st/3;
        painter->drawLine(QLine(pa0,pa1));
        painter->drawLine(QLine(pa0,pa2));
    }

    for(int i=0;i<outpN;i++)
    {
        QPoint pa0=GetOutCoor(i);
        if(putWasClicked==2 && i==clickedPutInd)
            painter->setPen(hlPen);
        else
            painter->setPen(linePen);
        painter->drawLine(pa0.x(),pa0.y(),pa0.x()-sign*g_st,pa0.y());
    }

    painter->setPen(linePen);


    //draw image
    if(ImgFlag)
    {
        if(imgDraw->size().width()!=(int)(width_img*g_st) && imgDraw->size().height()!=(int)(height*g_st))
            *imgDraw=img->scaled((int)(width_img*g_st),(int)(height*g_st),Qt::KeepAspectRatio);
        painter->drawPixmap(go_x+(x+1)*g_st,go_y+y*g_st,*imgDraw);
    }

    //draw display
    if(core_type==COT_DISPLAY)
    {
        if(data.size()>0)
        {
            if(data.at(0).size()>0)
            {
                QFont disp_font;
        #if defined(Q_OS_ANDROID)
                disp_font.setPixelSize(g_st*3.0/4.0+0.5);
        #else
                disp_font.setPixelSize(g_st);
        #endif
                painter->setFont(disp_font);
                int disp_x=data.at(0).back().second>=0?go_x+(x+1+0.5)*g_st:go_x+(x+1+0.2)*g_st;
                int disp_y=go_y+(y+height-0.5)*g_st;
                painter->drawText(disp_x,disp_y,QString::number(data.at(0).back().second,'f',4));
            }
        }
    }

    //draw borders
    if(wasClicked)
        painter->setPen(hlPen);
    else
        painter->setPen(linePen);

    QRectF Reg(go_x+(x+1)*g_st,go_y+y*g_st,width_img*g_st,height*g_st);
    painter->drawRect(Reg);

}

int Gr_DrawCore::GetX(){return isStatic?panel_grid_off_x+x*panel_grid_step:GRID_OFFSET_X+x*GRID_STEP;}

int Gr_DrawCore::GetY(){return isStatic?panel_grid_off_y+y*panel_grid_step:GRID_OFFSET_Y+y*GRID_STEP;}

int Gr_DrawCore::GetWidth(){return isStatic?width*panel_grid_step:width*GRID_STEP;}

int Gr_DrawCore::GetWidthImg(){return width_img*GRID_STEP;}

int Gr_DrawCore::GetHeight(){return isStatic?height*panel_grid_step:height*GRID_STEP;}

void Gr_DrawCore::SetX(int a){x=GridFit(a);}

void Gr_DrawCore::SetY(int a){y=GridFit(a);}

int Gr_DrawCore::GetNi(){return inpN;}

int Gr_DrawCore::GetNo(){return outpN;}

QPoint Gr_DrawCore::GetInpCoor(int ind)
{
    QPoint p;

    int gs=isStatic?panel_grid_step:GRID_STEP;
    if(!isRotated)
    {
        p.rx()=GetX();
        p.ry()=GetY()+(ind+1)*gs;
    }
    else
    {
        p.rx()=GetX()+GetWidth();
        p.ry()=GetY()+(ind+1)*gs;
    }
    p.rx()+=panel_dx;
    p.ry()+=panel_dy;
    return p;
}

QPoint Gr_DrawCore::GetOutCoor(int ind)
{
    QPoint p;

    int gs=isStatic?panel_grid_step:GRID_STEP;
    if(isRotated)
    {
        p.rx()=GetX();
        p.ry()=GetY()+(ind+1)*gs;
    }
    else
    {
        p.rx()=GetX()+GetWidth();
        p.ry()=GetY()+(ind+1)*gs;
    }
    p.rx()+=panel_dx;
    p.ry()+=panel_dy;
    return p;
}

void Gr_DrawCore::Rotate()
{
    isRotated=!isRotated;
}

void Gr_DrawCore::SetPos(int xx,int yy)
{
    panel_grid_off_x=xx+panel_grid_step;
    panel_grid_off_y=yy+panel_grid_step;
}

void Gr_DrawCore::SetSize(int ww,int hh)
{
    panel_grid_step=ww/6;
    double real_h=GetHeight();
    if(real_h>hh)
    {
        panel_grid_step=real_h/(height+2);
    }
}

bool Gr_DrawCore::Clicked(int xx, int yy)
{
    if(!isDrawn) return false;

    int g_st=isStatic?panel_grid_step:GRID_STEP;
    int go_x=(isStatic?panel_grid_off_x:GRID_OFFSET_X)+panel_dx;
    int go_y=(isStatic?panel_grid_off_y:GRID_OFFSET_Y)+panel_dy;
    if(xx>go_x+x*g_st && xx<go_x+x*g_st+width*g_st)
    {
        if(yy>go_y+y*g_st && yy<go_y+y*g_st+height*g_st)
        {
            return true;
        }
    }
    return false;
}

int Gr_DrawCore::InputClicked(int xx, int yy)
{
    if(!isDrawn) return -1;

    if(!isRotated)
    {
        if(xx-GetX()<GRID_STEP)
        {
            //check y conditions
            for (int j=0;j<GetNi();j++)
            {
                if(yy>GetY()+GRID_STEP/2+j*GRID_STEP && yy<GetY()+GRID_STEP/2+(j+1)*GRID_STEP)
                {
                    //input is clicked
                    return j;
                }
            }
        }
    }
    else
    {
        if(GetX()+GetWidth()-GRID_STEP<xx)
        {
            //check y conditions
            for (int j=0;j<GetNi();j++)
            {
                if(yy>GetY()+GRID_STEP/2+j*GRID_STEP && yy<GetY()+GRID_STEP/2+(j+1)*GRID_STEP)
                {
                    //input is clicked
                    return j;
                }
            }
        }
    }
    return -1;
}

int Gr_DrawCore::OutputClicked(int xx, int yy)
{
    if(!isDrawn) return -1;

    if(isRotated)
    {
        if(xx-GetX()<GRID_STEP)
        {
            //check y conditions
            for (int j=0;j<GetNo();j++)
            {
                if(yy>GetY()+GRID_STEP/2+j*GRID_STEP && yy<GetY()+GRID_STEP/2+(j+1)*GRID_STEP)
                {
                    //input is clicked
                    return j;
                }
            }
        }
    }
    else
    {
        if(GetX()+GetWidth()-GRID_STEP<xx)
        {
            //check y conditions
            for (int j=0;j<GetNo();j++)
            {
                if(yy>GetY()+GRID_STEP/2+j*GRID_STEP && yy<GetY()+GRID_STEP/2+(j+1)*GRID_STEP)
                {
                    //input is clicked
                    return j;
                }
            }
        }
    }
    return -1;
}

//============================================================================
Gr_Panel::Gr_Panel(int xx, int yy, int ww, int hh, drawable_types tt)
{
    class_type=DC_PANEL;

    x=xx;
    x0=x;

    y=yy;
    y0=y;

    width=ww;
    w0=width;

    height=hh;
    h0=height;

    type=tt;
}

void Gr_Panel::AddObj(Gr_Drawable* o)
{
    Objects.push_back(o);
}

void Gr_Panel::Clear()
{
    Objects.clear();
}

void Gr_Panel::Draw(QPainter *painter, QPaintEvent *event)
{
    if(!isDrawn)
        return;

    if(ImgFlag)
    {
        *imgDraw=img->scaled(width,height);
        painter->drawPixmap(x,y,*imgDraw);
    }

    for(int i=0;i<Objects.size();i++)
    {
        switch (Objects[i]->class_type)
        {
        case DC_DRAWABLE:
            Objects[i]->Draw(painter,event,x,y);
            break;
        case DC_DRAWCORE:
            ((Gr_DrawCore*)Objects[i])->Draw(painter,event,x,y);
            break;
        case DC_PANEL:
            ((Gr_Panel*)Objects[i])->Draw(painter,event,x,y);
            break;
        }

    }

    if(DrawBorder)
    {
        QPen pen;
        pen.setStyle(Qt::DashLine);
        pen.setColor(Qt::black);
        pen.setWidth(2);
        painter->setPen(pen);
        painter->drawRect(x,y,width,height);
    }
}

void Gr_Panel::Draw(QPainter *painter, QPaintEvent *event,int xx, int yy)
{
    //x=xx;
    //y=yy;
    //Draw(painter,event);
}

int Gr_Panel::ObjClicked(int xx,int yy)
{
    if(!isDrawn) return -1;

    for(int i=0;i<Objects.size();i++)
    {
        if(Objects[i]->class_type==DC_DRAWCORE)
        {
            if(((Gr_DrawCore*)Objects[i])->Clicked(xx,yy))
                return i;
        }
        else
        {
            if(Objects[i]->Clicked(xx-x,yy-y))
                return i;
        }
    }
    return -1;
}

void Gr_Panel::SetScalable()
{
    left=true;
    top=true;
    resize_cont=true;
    static_h=false;
    static_w=false;
}

void Gr_Panel::Scale(double sx,double sy)
{
    x=x0;
    y=y0;
    if(!static_w)
        width=w0*sx;
    if(!static_h)
        height=h0*sy;
    if(resize_cont)
    {
        int num_of_col=width/(Get1cm()*CONT_OBJ_SIZE);
        if(num_of_col==0) num_of_col=1;

        double space=0;
        double obj_w=width/num_of_col-(num_of_col+1)*space;
        double obj_h;
        //all elements have equal height, and equal width
        if(KeepRatio && Objects.size()>0)
        {
            //keep aspect ratio
            if(Objects[0]->class_type==DC_DRAWCORE)
            {
                obj_h=obj_w;
            }
            else
                obj_h=(Objects[0]->h0*1.0/Objects[0]->w0)*obj_w;
        }
        else
        {
            //error
            obj_h=1;
        }
        int xx,yy;
        for(int i=0;i<Objects.size();i++)
        {
            xx=obj_w*(i%num_of_col)+space*(i%num_of_col+1);
            yy=(int)(i/num_of_col)*obj_h+((int)(i/num_of_col+1))*space;

            switch (Objects[i]->class_type)
            {
            case DC_DRAWABLE:
                Objects[i]->SetSize(obj_w,obj_h);
                Objects[i]->SetPos(xx,yy);
                break;
            case DC_DRAWCORE:
                ((Gr_DrawCore*)Objects[i])->SetSize(obj_w,obj_h);
                ((Gr_DrawCore*)Objects[i])->SetPos(xx,yy);
                break;
            case DC_PANEL:
                ((Gr_Panel*)Objects[i])->SetSize(obj_w,obj_h);
                ((Gr_Panel*)Objects[i])->SetPos(xx,yy);
                break;
            }

            //Objects[i]->SetPos(x,y);
            //Objects[i]->SetSize(obj_w,obj_h);
        }
        height=yy+obj_h;
    }
}

void Gr_Panel::Move(double ww,double ww0, double wh,double wh0)
{
    /*if((bottom && top) || (left && right))*/

    if(bottom)
        y=wh-wh0+y0+h0-height;
    if(right)
        x=ww-ww0+x0+w0-width;
    if(mid_vert)
        y=wh/2-height/2;
    if(mid_horiz)
        x=ww/2-width/2;
}

//==============================================================================================
Gr_Window::Gr_Window()
{
    class_type=DC_WINDOW;

    x=0;
    y=0;
    width=1;
    height=1;
    type=DRWABLE_STATIC;
}

Gr_Window::Gr_Window(int xx, int yy, int ww, int hh, drawable_types tt)
{
    class_type=DC_WINDOW;

    x=xx;
    x0=x;
    y=yy;
    y0=y;
    width=ww;
    w0=width;
    height=hh;
    h0=height;
    type=tt;
}

void Gr_Window::Draw(QPainter *painter, QPaintEvent *event)
{
    if(!isDrawn)
        return;

    if(ImgFlag)
    {
        double ratio_w=width*1.0/img->width();
        double ratio_h=height*1.0/img->height();
        double dx=-100,dy=-100,sx=2,sy=2;
        if(ratio_w>ratio_h)
        {
            sx=ratio_w*1.1;
            sy=sx;
        }
        else if(ratio_w<ratio_h)
        {
            sx=ratio_h*1.1;
            sy=sx;
        }
        dx=-(sx*img->width()-width)/2;
        dy=-(sy*img->height()-height)/2;

        *imgDraw=img->scaled((int)(sx*img->width()+0.5),(int)(sy*img->height())+0.5);
        painter->drawPixmap((int)(dx+x),(int)(dy+y),*imgDraw);
    }
    if(class_type==DC_GRAPH)
    {
        ((Gr_Graph*)this)->Draw(painter,event);
    }
    for(int i=Panels.size()-1;i>=0;i--)
    {
        Panels[i]->Draw(painter,event);
    }
}

void Gr_Window::Upd(int ww,int hh)
{
    if(ww!=width || hh!=height)
    {
        if(class_type==DC_GRAPH) ((Gr_Graph*)this)->isPrepared=false;
        width=ww;
        height=hh;
        for(int i=0;i<Panels.size();i++)
        {
            Panels[i]->Scale(1.0*width/w0,1.0*height/h0);
            Panels[i]->Move(width,w0,height,h0);
        }
    }
}

pair<int,int> Gr_Window::ObjClicked(int xx,int yy)
{
    pair<int,int> p;
    for(int i=0;i<Panels.size();i++)
    {
        if(Panels[i]->Clicked(xx-x,yy-y))
        {
            p.first=i;
            p.second=Panels[i]->ObjClicked(xx-x,yy-y);
            return p;
        }
    }
    p.first=-1;
    p.second=-1;
    return p;
}

//=============================================================================
Gr_Graph::Gr_Graph()
{
    class_type=DC_GRAPH;
}

Gr_Graph::Gr_Graph(int xx, int yy, int ww, int hh,drawable_types tt)
{
    class_type=DC_GRAPH;
    x=xx;
    y=yy;
    width=ww;
    height=hh;
    type=tt;
}

void Gr_Graph::SetData(vector<vector<pair<double,double>>> *d)
{
    data_all=d;
    data=&d->at(0);
    isPrepared=false;
}

void Gr_Graph::Prepare(bool newflg)
{
    //clear plot transformed data
    plot.clear();
    plot2.clear();
    if(data!=NULL)
    {
        //find min and max values for axis
        if(data->size()>0)
        {
            if(newflg)
            {
                xmin=(*data)[0].first;
                xmax=xmin;
                ymin=(*data)[0].second;
                ymax=ymin;
                for(int i=0;i<data->size();i++)
                {
                   if(xmin>(*data)[i].first)
                       xmin=(*data)[i].first;
                   else if(xmax<(*data)[i].first)
                       xmax=(*data)[i].first;

                   if(ymin>(*data)[i].second)
                       ymin=(*data)[i].second;
                   else if(ymax<(*data)[i].second)
                       ymax=(*data)[i].second;
                }
                /*h_x=GraphRound((xmax-xmin)/(NumXLab-1));
                h_y=GraphRound((ymax-ymin)/(NumYLab-1));
                nod_x=NumOfDigits(h_x);
                nod_y=NumOfDigits(h_y);
                xmin=FancyFloor(xmin,nod_x);
                ymin=FancyFloor(ymin,nod_y);*/
            }
        }
        else
        {
            Gr_Logger().Get(LogError)<<"Not enough data to plot a graph"<<endl;
            return;
        }

        h_x=GraphRound((xmax-xmin)/(NumXLab-1));
        h_y=GraphRound((ymax-ymin)/(NumYLab-1));
        nod_x=NumOfDigits(h_x);
        nod_y=NumOfDigits(h_y);
        //cerr<<"//////////////////////"<<endl;
        //cerr<<"h:\t"<<h_x<<" "<<h_y<<endl;
        //cerr<<"nod:\t"<<nod_x<<" "<<nod_y<<endl;
        //cerr<<"min:\t"<<xmin<<" "<<ymin<<endl;
        //xmin=FancyFloor(xmin,nod_x);
        //ymin=FancyFloor(ymin,nod_y);

        //calc graph coordinates
        double x_lab_step=width/NumXLab;
        gra_x=x+x_lab_step;
        gra_w=width-x_lab_step-(x_lab_step/2)+x_lab_step/NUM_OF_CHAR_X;
        gra_y=y+x_lab_step/NUM_OF_CHAR_X;
        gra_h=height-6.0*x_lab_step/NUM_OF_CHAR_X;
        if(xmin!=xmax && ymin!=ymax)
        {
            //transform data to graph coordinates

            if(data_all->size()>1)
            {
                for(int j=0;j<data_all->size();j++)
                {
                    plot.clear();
                    for(int i=0;i<data_all->at(j).size();i++)
                    {
                        int x_p=(int)(gra_w*(data_all->at(j)[i].first-xmin)/(xmax-xmin)+0.5);
                        int y_p=(int)(gra_h-gra_h*(data_all->at(j)[i].second-ymin)/(ymax-ymin)+0.5);
                        //if(newflg || (x_p>=0 && x_p<=gra_w && y_p>=0 && y_p<=gra_h))
                        plot.push_back(pair<double,double>(x_p,y_p));
                    }
                    plot2.push_back(plot);
                }
            }
            else
            {
                for(int i=0;i<data->size();i++)
                {
                    int x_p=(int)(gra_w*((*data)[i].first-xmin)/(xmax-xmin)+0.5);
                    int y_p=(int)(gra_h-gra_h*((*data)[i].second-ymin)/(ymax-ymin)+0.5);
                    //if(newflg || (x_p>=0 && x_p<=gra_w && y_p>=0 && y_p<=gra_h))
                    plot.push_back(pair<double,double>(x_p,y_p));
                }
            }
        }

        //transform (0,0) to graph coordinates
        if(xmin!=xmax)
            x0=(int)(gra_w*(-xmin)/(xmax-xmin)+0.5);
        if(ymin!=ymax)
            y0=(int)(gra_h-gra_h*(-ymin)/(ymax-ymin)+0.5);
    }
    else
    {
        Gr_Logger().Get(LogError)<<"Zero pointer to data can't plot";
        return;
    }
    //set flag
    isPrepared=true;
}

void Gr_Graph::Draw(QPainter *painter, QPaintEvent *event)
{
    if(!isDrawn)
        return;

    //prepare graph
    if(!isPrepared)
        Prepare();

    //create pens
    //pen for data lines
    QPen lines_pen = QPen(Qt::red);
    lines_pen.setWidth(5);
    //pen for axis
    QPen axis_pen = QPen(Qt::black);
    axis_pen.setWidth(3);
    //pen for border
    QPen border_pen = QPen(Qt::black);
    border_pen.setWidth(2);
    border_pen.setStyle(Qt::DashLine);
    //pen for notches
    QPen lable_pen= QPen(Qt::black);
    lable_pen.setWidth(3);
    //pen for grid
    QPen grid_pen= QPen(Qt::black);
    grid_pen.setWidth(1);

    //fill bg
    painter->fillRect(x,y,width,height,Qt::GlobalColor::white);

    //if data is prepared
    if(isPrepared)
    {
        //Prepare axis values
        //calc label steps
        double graph_step_x=gra_w*1.0/(NumXLab-1);
        double graph_step_y=gra_h*1.0/(NumYLab-1);

        //calc font size
        int font_size=graph_step_x*2/(NUM_OF_CHAR_X+4);

        //calc data step
        /*double h_x=(xmax-xmin)/(NumXLab-1);
        double h_y=(ymax-ymin)/(NumYLab-1);
        h_x=GraphRound(h_x);
        h_y=GraphRound(h_y);
        cerr<<"step:"<<h_x<<" "<<h_y<<"\n";
        cerr<<"min:"<<xmin<<" "<<ymin<<"\n";*/
        double num;

        //set label font
#if defined(Q_OS_ANDROID)
        QFont label_font("Calibri",(int)(font_size*3.0/4.0+0.5));
#else
        QFont label_font("Calibri",font_size);
#endif

        painter->setFont(label_font);
        QFontMetrics fm = painter->fontMetrics();
        num=xmin;
        double l_p;//=gra_x;
        while(num<xmax)
        {
            num=FancyFloor(num,nod_x);
            QString lbl=QString::number(num,'G',5);
            l_p=gra_x+gra_w*(num-xmin)/(xmax-xmin);

            //draw labels
            painter->drawText(l_p-(fm.width(lbl))/2,gra_y+gra_h+font_size*2,lbl);

            //draw notches
            painter->setPen(lable_pen);
            painter->drawLine(l_p,gra_y+gra_h+font_size/2,l_p,gra_y+gra_h-font_size/2);

            //draw grid lines
            painter->setPen(grid_pen);
            painter->drawLine(l_p,gra_y+0,l_p,gra_y+gra_h);

            //make data and grid step
            //l_p+=graph_step_x;
            num+=h_x;
        }

        //draw Y-axis values
        //l_p=-gra_y-gra_h;
        //num=ymin;
        num=ymin;
        //cerr<<num<<"\n";
        double ly=0;
        //for(int i=0;i<NumYLab;i++)
        int nn=0;
        while((num<ymax || nn<NumYLab) && nn<NumYLab*2)
        {
            num=FancyFloor(num,nod_y);
            QString lbl=QString::number(num,'G',5);
            double y_notch=num;//lbl.toFloat();

            if(ymax==ymin)
            {
                ly=gra_y+gra_h/2;
            }
            else
            {
                ly=gra_y+(int)(gra_h-gra_h*(y_notch-ymin)/(ymax-ymin)+0.5);
            }

            if(1)//y_notch<=ymax && y_notch>=ymin)
            {
                //draw labels
                painter->drawText(gra_x-fm.width(lbl)-font_size,ly+font_size/2,lbl);

                //draw notches
                painter->setPen(lable_pen);
                painter->drawLine(gra_x-font_size/2,ly,gra_x+font_size/2,ly);

                //draw grid lines
                painter->setPen(grid_pen);
                painter->drawLine(gra_x,ly,gra_x+gra_w,ly);
            }

            //make data and grid step
            //l_p+=graph_step_y;
            num+=h_y;
            nn++;
        }

        //draw graph
        if(xmin!=xmax && ymin!=ymax)
        {
            painter->setPen(lines_pen);

            //limit number of graph segments
            long long step=plot.size()<GRAPH_MAX_LINES?(size_t)1:plot.size()/GRAPH_MAX_LINES;

            //set pens for multygraph
            vector<QPen> gr_pens;
            gr_pens.push_back(lines_pen);
            lines_pen.setColor(Qt::blue);
            gr_pens.push_back(lines_pen);
            lines_pen.setColor(Qt::green);
            gr_pens.push_back(lines_pen);
            lines_pen.setColor("orange");
            gr_pens.push_back(lines_pen);
            if(plot2.size()>1)
            {
                for(int j=0;j<plot2.size();j++)
                {
                    painter->setPen(gr_pens[j]);
                    for(int i=0;i<(long long)plot.size()-step;i+=step)
                    {
                        painter->drawLine(gra_x+plot2.at(j)[i].first,gra_y+plot2.at(j)[i].second,gra_x+plot2.at(j)[i+step].first,gra_y+plot2.at(j)[i+step].second);
                    }
                }
            }
            else
            {
                for(int i=0;i<(long long)plot.size()-step;i+=step)
                {
                    painter->drawLine(gra_x+plot[i].first,gra_y+plot[i].second,gra_x+plot[i+step].first,gra_y+plot[i+step].second);
                }
            }

            //draw axis lines
            painter->setPen(axis_pen);

            //axis X
            if(y0>=0 && y0<=gra_h)
            {
                painter->drawLine(gra_x+0,gra_y+y0,gra_x+gra_w,gra_y+y0);
                //draw arrow at the end
                painter->drawLine(gra_x+gra_w-font_size*2,gra_y+y0-font_size*3/4,gra_x+gra_w,gra_y+y0);
                painter->drawLine(gra_x+gra_w-font_size*2,gra_y+y0+font_size*3/4,gra_x+gra_w,gra_y+y0);
            }

            //axis Y
            if(x0>=0 && x0<=gra_w)
            {
                painter->drawLine(gra_x+x0,gra_y+0,gra_x+x0,gra_y+gra_h);
                //draw arrow at the end
                painter->drawLine(gra_x+x0-font_size*3/4,gra_y+font_size*2,gra_x+x0,gra_y);
                painter->drawLine(gra_x+x0+font_size*3/4,gra_y+font_size*2,gra_x+x0,gra_y);
            }
        }
        else
        {
            if(xmin!=xmax)
            {
                painter->setPen(lines_pen);
                painter->drawLine(gra_x,gra_y+gra_h/2,gra_x+gra_w,gra_y+gra_h/2);
                painter->setPen(axis_pen);
                painter->drawLine(x0,gra_y,x0,gra_y+gra_h);
            }
            else if(ymin!=ymax)
            {
                painter->setPen(lines_pen);
                painter->drawLine(gra_x+gra_w/2,gra_y,gra_x+gra_w/2,gra_y+gra_h);
                painter->setPen(axis_pen);
                painter->drawLine(gra_x,y0,gra_x+gra_w,y0);
            }
            else
            {
                //
            }
        }
    }
    else
    {
        //no data
    }

    //draw borders
    if(DrawBorder)
    {
        painter->setPen(border_pen);
        QRectF Reg(x,y,width,height);
        painter->drawRect(Reg);
    }
}
