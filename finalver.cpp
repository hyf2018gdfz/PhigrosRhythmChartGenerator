#include<bits/stdc++.h>
#include<windows.h>
using namespace std;
char ReadLoc[155],OutputLoc[155];

void ChangeStingToChar(string str,char *ch){
    int len = str.length();
    for(int i = 0;i<len;++i) ch[i] = str[i];
}
struct EventTime{
    int start,wh,div = 1;//第start拍，第wh/div个音
    void ScanIn(string str){
        char chstr[105];
        ChangeStingToChar(str,chstr);
        sscanf(chstr,"               \"startTime\" : [ %d, %d, %d ],",&start,&wh,&div);
        if(wh == 5 && div == 32){
            wh = 1,div = 6;
        }else if(wh == 11 && div == 32){
            wh = 1,div = 3;
        }else if(wh == 21 && div == 32){
            wh = 2,div = 3;
        }else if(wh == 27 && div == 32){
            wh = 5,div = 6;
        }
    }
    void PrintOut(){
        printf("[%d, %d, %d]\n",start,wh,div);
    }
    bool operator ==(const EventTime &ano){
        if(start == ano.start && wh == ano.wh && div == ano.div) return true;
        else return false;
    }  
};
struct Notes{
    EventTime HappenTime;
    int typ;
    void ScanIn(string str){
        char chstr[105];
        ChangeStingToChar(str,chstr);
        sscanf(chstr,"               \"type\" : %d,",&typ);
        if(typ == 1 || typ == 2) typ = 1;
        else if(typ == 3) typ = 2;
        else typ = 0;
    }
    void PrintOut(){
        HappenTime.PrintOut();
        printf("type = %d\n",typ);
    }
    bool operator ==(const Notes &ano){
        if(typ == ano.typ && HappenTime == ano.HappenTime) return true;
        else return false;
    }
}SongNote[5005],AfterSelect[5005];
bool cmp(Notes x,Notes y){
    double xtim = x.HappenTime.start+1.0*x.HappenTime.wh/x.HappenTime.div;
    double ytim = y.HappenTime.start+1.0*y.HappenTime.wh/y.HappenTime.div;
    if(xtim == ytim) return x.typ<y.typ;
    else return xtim<ytim;
}
int TotalNotes,SelectNotes;
#define ot(p) outFile.write((const char*)&p,1)
int curbpm = 225;
int ps[105] = {0,0,0,6,0,0,0,1,0,120};
int ps2[105] = {0,0,0,0,0,255,88,4,4,2,24,8,0,255,89,2,0,0,0,255,81,3,0,0,0};
int tickhi(int num){
    return ((num>>7)&127)|128;
}
int ticklo(int num){
    return num&127;
}
int hpt_to_tick(EventTime num){
    return (num.start-1)*120+num.wh*120/num.div;
}
/*在selectnotes中，1代表红蓝双押，2代表红键，3代表蓝键*/
void output_midi(){
    int countbytes = 6;//休止到第一个键
    vector<int> vec;//存储二进制notes
    vec.clear();
    vec.push_back(0);
    vec.push_back(128),vec.push_back(60),vec.push_back(0);
    vec.push_back(tickhi(hpt_to_tick(AfterSelect[1].HappenTime)+120));
    vec.push_back(ticklo(hpt_to_tick(AfterSelect[1].HappenTime)+120));
    for(int i = 1;i<=SelectNotes;++i){

        int delta_tick;
        if(i == SelectNotes) delta_tick = 480;
        else delta_tick = hpt_to_tick(AfterSelect[i+1].HappenTime)-hpt_to_tick(AfterSelect[i].HappenTime);
        
        if(AfterSelect[i].typ == 1){
            vec.push_back(144),vec.push_back(60),vec.push_back(64);
            vec.push_back(ticklo(0));
            vec.push_back(144),vec.push_back(72),vec.push_back(64);
            vec.push_back(tickhi(delta_tick)),vec.push_back(ticklo(delta_tick));
            vec.push_back(128),vec.push_back(60),vec.push_back(0);
            vec.push_back(ticklo(0));
            vec.push_back(128),vec.push_back(72),vec.push_back(0);
            vec.push_back(ticklo(0));
            countbytes += 17;
        }else if(AfterSelect[i].typ == 2){
            vec.push_back(144),vec.push_back(72),vec.push_back(64);
            vec.push_back(tickhi(delta_tick)),vec.push_back(ticklo(delta_tick));
            vec.push_back(128),vec.push_back(72),vec.push_back(0);
            vec.push_back(ticklo(0));
            countbytes += 9;
        }else if(AfterSelect[i].typ == 3){
            vec.push_back(144),vec.push_back(60),vec.push_back(64);
            vec.push_back(tickhi(delta_tick)),vec.push_back(ticklo(delta_tick));
            vec.push_back(128),vec.push_back(60),vec.push_back(0);
            vec.push_back(ticklo(0));
            countbytes += 9;
        }
    }
    //printf("countbytes = %d\n",countbytes+24);
    ofstream outFile(OutputLoc, ios::out | ios::binary);
    //head
    outFile <<"\x4d\x54\x68\x64";
    for(int i = 0;i<10;++i) ot(ps[i]);
    //body
    outFile<<"\x4d\x54\x72\x6b";
    ps2[0] = (countbytes+24)/16777216;
    ps2[1] = ((countbytes+24)/65536)&255;
    ps2[2] = ((countbytes+24)/256)&255;
    ps2[3] = (countbytes+24)&255;
    int curtick = int(60.0*1000000/curbpm+0.5);
    ps2[22] = (curtick/65536)&255;
    ps2[23] = (curtick/256)&255;
    ps2[24] = curtick&255;
    for(int i = 0;i<25;++i) ot(ps2[i]);
    for(int i = 0;i<countbytes;++i){
        outFile<<char(vec[i]);
    }
    outFile<<char(0xff)<<char(0x2f)<<char(0x00);
    outFile.close();
}
int main(){
    freopen("loc.txt","r",stdin);
    scanf("%s",ReadLoc);
    scanf("%s",OutputLoc);
    freopen(ReadLoc,"r",stdin);
    string Readstr;
    while(true){
        if(!getline(cin,Readstr)) break;
        if(Readstr.substr(0,25) == "               \"above\" : "){
            TotalNotes++;
            while(getline(cin,Readstr)){
                if(Readstr.substr(0,29) == "               \"startTime\" : "){
                    break;
                }
            }
            SongNote[TotalNotes].HappenTime.ScanIn(Readstr);
            while(getline(cin,Readstr)){
                if(Readstr.substr(0,24) == "               \"type\" : "){
                    break;
                }
            }
            SongNote[TotalNotes].ScanIn(Readstr);
        }else{

        }
    }
    sort(SongNote+1,SongNote+TotalNotes+1,cmp);
    for(int i = 1;i<=TotalNotes;){
        int j = i;
        bool blue_ex = false,red_ex = false;
        for(;j<=TotalNotes;++j){
            if(!(SongNote[i].HappenTime == SongNote[j].HappenTime)){
                break;
            }
            if(SongNote[j].typ == 1) blue_ex = true;
            if(SongNote[j].typ == 2) red_ex = true;
        }
        if(blue_ex && red_ex){
            AfterSelect[++SelectNotes] = SongNote[i];
            AfterSelect[SelectNotes].typ = 1;
        }else if(!blue_ex && red_ex){
            AfterSelect[++SelectNotes] = SongNote[i];
            AfterSelect[SelectNotes].typ = 2;
        }else if(blue_ex && !red_ex){
            AfterSelect[++SelectNotes] = SongNote[i];
            AfterSelect[SelectNotes].typ = 3;
        }
        i = j;
    }
    output_midi();
    return 0;
}