/*
    BeaconGenerator
    for DGSN!

        please feel informed, that this code is
        QUICK and DIRTY
        and is not meant to look nice and readable!
        Not that you are informed, have fun! :D
        - andreas
*/


#include <iostream>
#include <math.h>
#include <time.h>

// write to file
#include <fstream>
#include <sstream>

using namespace std;

const double PI  = 3.141592653589793238463;
const double c  = 300000000.0;

int main()
{

    // sim
    double dt = 0.02; //[s]
    // earth, one
    double G = 6.673848*pow(10,-11);
    double mEarth = 5.97219*pow(10, 24);
    double rEarth = 6371000.0;

    double elevation_min = 20.0;

    //ground station deployment on grid
    int gs = 40; // per quarter arc
    int gs_nodes_per_lat[gs];
    double gs_circum_lat = 2*PI*rEarth/((gs-1)*4);

    double circum[gs];
    double gs_circum_lat_corr[gs];

    //calcs for only a 4th arc
    for(int i=0; i<gs; i++){
        circum[i] = 2.0/4.0*PI*cos((360.0/4.0)/(gs-1)*i*PI/180.0)*rEarth;
        double segments = round(circum[i]/gs_circum_lat);
        gs_nodes_per_lat[i] = segments+1;
        gs_circum_lat_corr[i] = circum[i]/(gs_nodes_per_lat[i]-1);

        cout << gs_circum_lat_corr[i] << " " << gs_circum_lat << endl;
    }

    int sizea = gs*2;
    int sizeb = (gs-1)*4;
    double gs_longlat[2][sizea][sizeb];
    int gs_nodes_lat[sizea];
    cout << "test " << gs_longlat << endl;

    for(int i=0; i<gs; i++){
        for(int j=0; j<(gs_nodes_per_lat[i]*2-1)*2-2; j++){
            //if(gs_nodes_per_lat[i]>1){
                gs_longlat[0][gs+i-1][j] = 360.0/(4.0*(gs_nodes_per_lat[i]-1))*j;
                gs_longlat[0][gs-i-1][j] = 360.0/(4.0*(gs_nodes_per_lat[i]-1))*j;
                gs_longlat[1][gs+i-1][j] = 90.0/(gs-1)*i;
                gs_longlat[1][gs-i-1][j] = -1.0*gs_longlat[1][gs+i-1][j];
            //}
            gs_nodes_lat[gs-i-1] = (gs_nodes_per_lat[i]-1)*4;
            gs_nodes_lat[gs+i-1] = (gs_nodes_per_lat[i]-1)*4;
            //cout << i << " " << j << " pos " << gs_longlat[0][gs+i][j] << " " << gs_longlat[1][gs+i][j] << " " << gs_longlat[1][gs-i][j] << endl;
        }
    }

    gs_longlat[0][2*gs-2][0] = 0.0;
    gs_longlat[1][2*gs-2][0] = 90.0;
    gs_longlat[0][0][0] = 0.0;
    gs_longlat[1][0][0] = -90.0;
    gs_nodes_lat[2*gs-2] = 1;
    gs_nodes_lat[0] = 1;
    //cout << gs-1 << " " << 0 << "pos " << gs_longlat[0][gs-1][0] << " " << gs_longlat[1][gs-1][0]  << " " << gs_longlat[1][0][0]<< endl;

    for(int i=0; i<gs*2-1; i++){
        for(int j=0; j<gs_nodes_lat[i]; j++){
            //cout << "hh " << i << " " << j << " pos " << gs_nodes_lat[i] << " " << gs_longlat[0][i][j] << " " << gs_longlat[1][i][j] << endl;
        }
    }
    // there is still something wrong with the gs distribution, but it works now, somehow

    double gs_posxyz[3][sizea][sizeb];
    double gs_poslength[1][sizea][sizeb];
    for(int i=0; i<gs*2-1; i++){
        for(int j=0; j<gs_nodes_lat[i]; j++){
             gs_posxyz[0][i][j] = rEarth*sin(gs_longlat[0][i][j]*PI/180.0)*cos(gs_longlat[1][i][j]*PI/180.0);
             gs_posxyz[1][i][j] = rEarth*cos(gs_longlat[0][i][j]*PI/180.0)*cos(gs_longlat[1][i][j]*PI/180.0);
             gs_posxyz[2][i][j] = rEarth*sin(gs_longlat[1][i][j]*PI/180.0);
             gs_poslength[0][i][j] = sqrt(pow(gs_posxyz[0][i][j],2.0)+pow(gs_posxyz[1][i][j],2.0)+pow(gs_posxyz[2][i][j],2.0));

             //cout << "zz " << i << " " << j << " pos " << gs_posxyz[0][i][j] << " " << gs_posxyz[1][i][j] << " " << gs_posxyz[2][i][j] << endl;

        }
    }

    double gssat_vecxyz[3][sizea][sizeb];
    double gssat_length[1][sizea][sizeb];

    // beacons, many
    double beacon_m[1];
    beacon_m[0] = 100.0; //[kg]

    double beacon_r[1][3]; //x,y,z [m]
    beacon_r[0][0] = rEarth+200000.0;
    beacon_r[0][1] = 0.0;
    beacon_r[0][2] = 0.0;

    double beacon_v[1][3]; //vx,vy,vz [m/s]
    beacon_v[0][0] = 0.0;
    beacon_v[0][1] = 7700.0;
    beacon_v[0][2] = 0.0;

    double beacon_a[1][3]; //ax,ay,az [m/s]
    beacon_a[0][0] = 0.0;
    beacon_a[0][1] = 0.0;
    beacon_a[0][2] = 0.0;

    // calculations, oh dear!
    double runtime = 0.0;
    double t_jump = 5.0;
    double runtime_stop = runtime + t_jump;

    unsigned long int sec0= time(NULL);
    unsigned long int sec1= sec0+t_jump;
    //cout << sec0 << endl;

    ofstream outputFile;

    double gssat_angle[1][sizea][sizeb];
    double gssat_runtime[1][sizea][sizeb];

    while(runtime<=100.0*60){
        beacon_a[0][0] = G*mEarth*(0.0-beacon_r[0][0])/pow(sqrt(pow((0.0-beacon_r[0][0]),2)+pow((0.0-beacon_r[0][1]),2)+pow((0.0-beacon_r[0][2]),2)),3);
        beacon_a[0][1] = G*mEarth*(0.0-beacon_r[0][1])/pow(sqrt(pow((0.0-beacon_r[0][0]),2)+pow((0.0-beacon_r[0][1]),2)+pow((0.0-beacon_r[0][2]),2)),3);
        beacon_a[0][2] = G*mEarth*(0.0-beacon_r[0][2])/pow(sqrt(pow((0.0-beacon_r[0][0]),2)+pow((0.0-beacon_r[0][1]),2)+pow((0.0-beacon_r[0][2]),2)),3);

        beacon_v[0][0] = beacon_v[0][0] + beacon_a[0][0]*dt;
        beacon_v[0][1] = beacon_v[0][1] + beacon_a[0][1]*dt;
        beacon_v[0][2] = beacon_v[0][2] + beacon_a[0][2]*dt;

        beacon_r[0][0] = beacon_r[0][0] + beacon_v[0][0]*dt;
        beacon_r[0][1] = beacon_r[0][1] + beacon_v[0][1]*dt;
        beacon_r[0][2] = beacon_r[0][2] + beacon_v[0][2]*dt;

        runtime = runtime + dt;

        for(int i=0; i<gs*2-1; i++){
            for(int j=0; j<gs_nodes_lat[i]; j++){
                gssat_vecxyz[0][i][j] = beacon_r[0][0]-gs_posxyz[0][i][j];
                gssat_vecxyz[1][i][j] = beacon_r[0][1]-gs_posxyz[1][i][j];
                gssat_vecxyz[2][i][j] = beacon_r[0][2]-gs_posxyz[2][i][j];

                gssat_length[0][i][j] = sqrt(pow(gssat_vecxyz[0][i][j],2.0)+pow(gssat_vecxyz[1][i][j],2.0)+pow(gssat_vecxyz[2][i][j],2.0));
                gssat_runtime[0][i][j] = gssat_length[0][i][j]/c;
                //cout << "vec " << gssat_vecxyz[0][i][j] << " " << gssat_vecxyz[1][i][j] << " " << gssat_vecxyz[2][i][j] << endl;
            }
        }

        if(runtime>=runtime_stop){
            //cout << runtime << ";" << beacon_a[0][0] << ";" << beacon_a[0][1] << ";" << beacon_a[0][2] << ";" << beacon_v[0][0] << ";" << beacon_v[0][1] << ";" << beacon_v[0][2] << ";" << beacon_r[0][0] << ";" << beacon_r[0][1] << ";" << beacon_r[0][2] << endl;
            cout <<"beacon signal sent @ " << runtime << " seconds" << endl;
            // the following will be stored in each file!
            cout << "gspx;gspy;gspz;gsrectime;satstamp;satid(optional);satdata(optional);gsid(optional)" << endl;
            outputFile.open("satposition.sat", ios::app);
            outputFile.precision(12);
            outputFile << beacon_r[0][0] << ";" << beacon_r[0][1] << ";" << beacon_r[0][2] << ";" << runtime << ";" << endl;
            outputFile.close();

            for(int i=0; i<gs*2-1; i++){
                for(int j=0; j<gs_nodes_lat[i]; j++){
                    //cout << "vec " << gssat_vecxyz[0][i][j] << " " << gssat_vecxyz[1][i][j] << " " << gssat_vecxyz[2][i][j] << " " << gssat_length[0][i][j] << endl;
                    gssat_angle[0][i][j] = 180.0/PI * acos((gssat_vecxyz[0][i][j]*gs_posxyz[0][i][j]+gssat_vecxyz[1][i][j]*gs_posxyz[1][i][j]+gssat_vecxyz[2][i][j]*gs_posxyz[2][i][j])/(gssat_length[0][i][j]*gs_poslength[0][i][j]));

                    //filter for elevation and links going through earth
                    if(gssat_angle[0][i][j]<=90.0-elevation_min){
                        cout << "gs-reception: ";
                        cout << gs_posxyz[0][i][j] << ";" << gs_posxyz[1][i][j] << ";" << gs_posxyz[2][i][j] << ";";
                        cout << gssat_runtime[0][i][j] << ";";
                        cout << runtime << ";";
                        cout << "satid:1;" << "gsid:" << i << ":" << j << endl;

                        string pName = "station_";
                        string tempi;          //The string
                        string tempj;          //The string
                        ostringstream temp1;  //temp as in temporary
                        temp1<<i;
                        tempi=temp1.str();      //str is temp as string
                        ostringstream temp2;  //temp as in temporary
                        temp2<<j;
                        tempj=temp2.str();      //str is temp as string

                        pName = pName +"_"+tempi+"_"+tempj+"_"+".gsd";
                        char* filename = (char*) pName.c_str();

                        outputFile.open(filename, ios::app);
                        outputFile.precision(12);
                        outputFile << gs_posxyz[0][i][j] << ";" << gs_posxyz[1][i][j] << ";" << gs_posxyz[2][i][j] << ";" << gssat_runtime[0][i][j] << ";" << runtime << ";" << "satid:1;" << "gsid:" << i << ":" << j << endl;
                        outputFile.close();
                    }
                }
            }
            cout << " " << endl;

            runtime_stop = runtime + t_jump;

            while(time(NULL)<=sec1){
                // notn, hangin' around, wastin' time!
            }
            sec1= time(NULL)+t_jump;

        }

    }
    cout << "Done!\n";
    return 0;
}
