int calculate_gps_time(int year,int month,int day,int hour,int minute,int second) {
    int total = day-6;
    for (int i=1980;i<year;i++) {
        total+=365;
        if (i%4==0) total++;
    }
    int days_in_month[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    for (int i=0;i<month-1;i++) {
        total+=days_in_month[i];
    }
    return total/7*604800+total%7*86400+hour*3600+minute*60+second;
}

Arguments parse_args(int argc, char *argv[]) {
    Arguments args;
    int Y0 = (argv[2][0]-'0')*1000+(argv[2][1]-'0')*100+(argv[2][2]-'0')*10+(argv[2][3]-'0');
    int M0 = (argv[2][5]-'0')*10+(argv[2][6]-'0');
    int D0 = (argv[2][8]-'0')*10+(argv[2][9]-'0');
    int h0 = (argv[2][11]-'0')*10+(argv[2][12]-'0');
    int m0 = (argv[2][14]-'0')*10+(argv[2][15]-'0');
    int s0 = (argv[2][17]-'0')*10+(argv[2][18]-'0');
    args.time0 = calculate_gps_time(Y0,M0,D0,h0,m0,s0);
    int Y1 = (argv[3][0]-'0')*1000+(argv[3][1]-'0')*100+(argv[3][2]-'0')*10+(argv[3][3]-'0');
    int M1 = (argv[3][5]-'0')*10+(argv[3][6]-'0');
    int D1 = (argv[3][8]-'0')*10+(argv[3][9]-'0');
    int h1 = (argv[3][11]-'0')*10+(argv[3][12]-'0');
    int m1 = (argv[3][14]-'0')*10+(argv[3][15]-'0');
    int s1 = (argv[3][17]-'0')*10+(argv[3][18]-'0');
    args.time1 = calculate_gps_time(Y1,M1,D1,h1,m1,s1);
    args.timei = atoi(argv[4]);
    args.phi = atof(argv[5])/180.0*M_PI;
    args.lambda = atof(argv[6])/180.0*M_PI;
    args.h = atof(argv[7]);
    args.mask = atof(argv[8])/180.0*M_PI;
    std::vector<int> specific;
    for (int i =10;i<argc;i++){
        if (strcmp(argv[i],"GPS")==0) {
            specific.insert(specific.end(),{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,23,24,25,26,27,28,29,30,31,32});
        } else if (strcmp(argv[i],"GLONASS")==0) {
            specific.insert(specific.end(),{38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61});
        } else if (strcmp(argv[i],"QZSS")==0) {
            specific.insert(specific.end(),{112,113,114,117});
        } else if (strcmp(argv[i],"GALILEO")==0) {
            specific.insert(specific.end(),{201,202,203,204,205,207,208,209,210,211,212,213,214,215,218,219,221,224,225,226,227,230,231,233,234,236});
        } else if (strcmp(argv[i],"BEIDOU")==0) {
            specific.insert(specific.end(),{264,265,266,267,268,269,274,275,276,277,279,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,366,367,368,369,370,371,372,373,374,384,385,386,387,388,389});
        } else {
            specific.push_back(atoi(argv[i]));
        }  
    }
    args.sids=specific;
    return args;
};

std::vector<Satellite> parse_almanac(char *filename) {
    std::vector<Satellite> SatelliteData;
    FILE *input = fopen(filename,"r");
    char cnew='\0', cold='\0', buffer[10]={'\0'};
    int bufferPointer=0, linePointer=0, columnPointer=0;
    float anteroom[6][13];

    while ((cnew=fgetc(input))!=EOF) {
        if (cnew=='\r'||(cnew==cold&&cnew=='\n')) continue;
        if (cnew!='\n') {
            buffer[bufferPointer]=cnew;
            bufferPointer++;
        } else {
            linePointer++;
            columnPointer=0;
        }
        cold=cnew;
        if (bufferPointer>=10) {
            sscanf(buffer,"%f",&anteroom[columnPointer][linePointer%13]);
            if (linePointer%13==12) {
                Satellite sat;
                sat.sid = anteroom[columnPointer][0];
                sat.health = anteroom[columnPointer][1];
                sat.e = anteroom[columnPointer][2];
                sat.sqrta = (double)((int)(anteroom[columnPointer][3]*10)/10.0);
                sat.Omega = anteroom[columnPointer][4]/180.0*M_PI;
                sat.omega = anteroom[columnPointer][5]/180.0*M_PI;
                sat.anomaly = anteroom[columnPointer][6]/180.0*M_PI;
                sat.toa = anteroom[columnPointer][7];
                sat.i = (anteroom[columnPointer][8]+54.0)/180.0*M_PI;
                sat.OmegaDT = anteroom[columnPointer][9]/1000.0/180.0*M_PI;
                sat.sco = anteroom[columnPointer][10];
                sat.scd = anteroom[columnPointer][11];
                sat.week = anteroom[columnPointer][12];
                SatelliteData.push_back(sat);
            }
            columnPointer=columnPointer+1>=6?0:columnPointer+1;
            bufferPointer=0;
        }
    }
    fclose(input);
    return SatelliteData;
};

Receiver locate_receiver(float phi, float lambda, float height, float mask) {
    Receiver rec;

    rec.phi = phi;
    rec.lambda = lambda;
    rec.height = height;
    rec.mask = mask;

    float N = 6378137.0/sqrt(1-0.00669438002290*pow(sin(phi),2));

    rec.X = (N+height)*cos(phi)*cos(lambda);
    rec.Y = (N+height)*cos(phi)*sin(lambda);
    rec.Z = (N*(1-0.00669438002290)+height)*sin(phi);

    rec.rneu[0][0] = -sin(phi)*cos(lambda);rec.rneu[0][1] = -sin(lambda);rec.rneu[0][2] = cos(phi)*cos(lambda);
    rec.rneu[1][0] = -sin(phi)*sin(lambda);rec.rneu[1][1] = cos(lambda);rec.rneu[1][2] = cos(phi)*sin(lambda);
    rec.rneu[2][0] = cos(phi);rec.rneu[2][1] = 0.0;rec.rneu[2][2] = sin(phi);

    return rec;
}

Position get_position(Satellite sat, Receiver rec, int time) {
    Position pos;
    double almanac_time = (double)(sat.week*604800.0+sat.toa);
    double epoch = (double)time - almanac_time;
    double a = pow(sat.sqrta,2);
    double n = sqrt(MI)/sat.sqrta/sat.sqrta/sat.sqrta;
    double anomaly_k = sat.anomaly+n*epoch;
    double ek = anomaly_k;
    int counter;
    while (abs(anomaly_k+sat.e*sin(ek)-ek)>pow(10,-12)) {
        ek = anomaly_k+sat.e*sin(ek);
        counter++;
        if (counter>=10) break;
    } // TO CHECK
    double vk = atan2(sqrt(1-sat.e*sat.e)*sin(ek),cos(ek)-sat.e); // TO CHECK
    double Phi_k = vk + sat.omega;
    double rk = a*(1-sat.e*cos(ek));
    double xk = rk*cos(Phi_k);
    double yk = rk*sin(Phi_k);
    double Omega_k = sat.Omega+(sat.OmegaDT-OMEGA_E)*epoch-OMEGA_E*sat.toa;
    double Xk = xk*cos(Omega_k)-yk*cos(sat.i)*sin(Omega_k);
    double Yk = xk*sin(Omega_k)+yk*cos(sat.i)*cos(Omega_k);
    double Zk = yk*sin(sat.i);
    double phi = atan((Zk/sqrt(Xk*Xk+Yk*Yk))*pow(1-sat.e*sat.e,-1));
    double h=0.0;
    while (true) {
        double N = a/sqrt(1-sat.e*sat.e*sin(phi)*sin(phi));
        h = sqrt(Xk*Xk+Yk*Yk)/cos(phi)-N;
        double phik = atan((Zk/sqrt(Xk*Xk+Yk*Yk))*pow(1-sat.e*sat.e*(N/(N+h)),-1));
        if (abs(phik-phi)<0.0001) {phi=phik;break;}
        phi = phik;
    }
    double lambda = atan2(Yk,Xk);

    pos.X=Xk;
    pos.Y=Yk;
    pos.Z=Zk;
    pos.phi=phi/M_PI*180.0;
    pos.lambda=lambda/M_PI*180.0;
    pos.height=h;

    double vX = pos.X-rec.X;
    double vY = pos.Y-rec.Y;
    double vZ = pos.Z-rec.Z;

    double mn = rec.rneu[0][0]*vX+rec.rneu[1][0]*vY+rec.rneu[2][0]*vZ;
    double me = rec.rneu[0][1]*vX+rec.rneu[1][1]*vY+rec.rneu[2][1]*vZ;
    double mu = rec.rneu[0][2]*vX+rec.rneu[1][2]*vY+rec.rneu[2][2]*vZ;

    pos.azimuth = atan2(me,mn)/M_PI*180.0;
    pos.elevation = asin(mu/sqrt(mn*mn+me*me+mu*mu))/M_PI*180.0;

    pos.visible = (pos.elevation<=90.0&&pos.elevation>=rec.mask/M_PI*180.0) ? true : false;

    if (sat.sid>0&&sat.sid<=37) pos.system = 0;//gps
    else if (sat.sid>=38&&sat.sid<=64) pos.system=1;//glonass
    else if (sat.sid>=111&&sat.sid<=118) pos.system=4;//qzss
    else if (sat.sid>=201&&sat.sid<=263) pos.system=2;// galileo
    else if (sat.sid>=264) pos.system = 3;//beidou
    else pos.system=5; // probably irnss
    return pos;
}

std::vector<double> genArow(Position pos, Receiver rec) {
    double parX = -1*(pos.X-rec.X)/sqrt((pos.X-rec.X)*(pos.X-rec.X)+(pos.Y-rec.Y)*(pos.Y-rec.Y)+(pos.Z-rec.Z)*(pos.Z-rec.Z));
    double parY = -1*(pos.Y-rec.Y)/sqrt((pos.X-rec.X)*(pos.X-rec.X)+(pos.Y-rec.Y)*(pos.Y-rec.Y)+(pos.Z-rec.Z)*(pos.Z-rec.Z));
    double parZ = -1*(pos.Z-rec.Z)/sqrt((pos.X-rec.X)*(pos.X-rec.X)+(pos.Y-rec.Y)*(pos.Y-rec.Y)+(pos.Z-rec.Z)*(pos.Z-rec.Z));
    std::vector<double> row = {parX,parY,parZ,1.0};
    return row;
}

DOPS get_DOP(std::vector<std::vector<double>> A,Receiver rec) {
    DOPS dops;
    dops.sat4 = (A.size()>=4) ? true : false;
    if (!dops.sat4) return dops;
    std::vector<std::vector<double>> ATA;
    for (int i =0;i<4;i++) {
        std::vector<double> row;
        for (int j = 0;j<4;j++) {
            double element = 0.0;
            for (int n = 0;n<A.size();n++) {
                element += A[n][i]*A[n][j]; // alfa * beta
            }
            row.push_back(element);
        }
        ATA.push_back(row);
    }
    double detATA = 0.0;
    detATA += ATA[0][0]*ATA[1][1]*ATA[2][2]*ATA[3][3]+ATA[0][0]*ATA[1][2]*ATA[2][3]*ATA[3][1]+ATA[0][0]*ATA[1][3]*ATA[2][1]*ATA[3][2];
    detATA -= ATA[0][0]*ATA[1][3]*ATA[2][2]*ATA[3][1]+ATA[0][0]*ATA[1][2]*ATA[2][1]*ATA[3][3]+ATA[0][0]*ATA[1][1]*ATA[2][3]*ATA[3][2];
    detATA -= ATA[0][1]*ATA[1][0]*ATA[2][2]*ATA[3][3]+ATA[0][2]*ATA[1][0]*ATA[2][3]*ATA[3][1]+ATA[0][3]*ATA[1][0]*ATA[2][1]*ATA[3][2];
    detATA += ATA[0][3]*ATA[1][0]*ATA[2][2]*ATA[3][1]+ATA[0][2]*ATA[1][0]*ATA[2][1]*ATA[3][3]+ATA[0][1]*ATA[1][0]*ATA[2][3]*ATA[3][2];
    detATA += ATA[0][1]*ATA[1][2]*ATA[2][0]*ATA[3][3]+ATA[0][2]*ATA[1][3]*ATA[2][0]*ATA[3][1]+ATA[0][3]*ATA[1][1]*ATA[2][0]*ATA[3][2];
    detATA -= ATA[0][3]*ATA[1][2]*ATA[2][0]*ATA[3][1]+ATA[0][2]*ATA[1][1]*ATA[2][0]*ATA[3][3]+ATA[0][1]*ATA[1][3]*ATA[2][0]*ATA[3][2];
    detATA -= ATA[0][1]*ATA[1][2]*ATA[2][3]*ATA[3][0]+ATA[0][2]*ATA[1][3]*ATA[2][1]*ATA[3][0]+ATA[0][3]*ATA[1][1]*ATA[2][2]*ATA[3][0];
    detATA += ATA[0][3]*ATA[1][2]*ATA[2][1]*ATA[3][0]+ATA[0][2]*ATA[1][1]*ATA[2][3]*ATA[3][0]+ATA[0][1]*ATA[1][3]*ATA[2][2]*ATA[3][0];

    std::vector<std::vector<double>> adjATA = {{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0}};

    adjATA[0][0] = ATA[1][1]*ATA[2][2]*ATA[3][3]+ATA[1][2]*ATA[2][3]*ATA[3][1]+ATA[1][3]*ATA[2][1]*ATA[3][2]-ATA[1][3]*ATA[2][2]*ATA[3][1]-ATA[1][2]*ATA[2][1]*ATA[3][3]-ATA[1][1]*ATA[2][3]*ATA[3][2];
    adjATA[0][1] = ATA[0][3]*ATA[2][2]*ATA[3][1]+ATA[0][2]*ATA[2][1]*ATA[3][3]+ATA[0][1]*ATA[2][3]*ATA[3][2]-ATA[0][1]*ATA[2][2]*ATA[3][3]-ATA[0][2]*ATA[2][3]*ATA[3][1]-ATA[0][3]*ATA[2][1]*ATA[3][2];
    adjATA[0][2] = ATA[0][1]*ATA[1][2]*ATA[3][3]+ATA[0][2]*ATA[1][3]*ATA[3][1]+ATA[0][3]*ATA[1][1]*ATA[3][2]-ATA[0][3]*ATA[1][2]*ATA[3][1]-ATA[0][2]*ATA[1][1]*ATA[3][3]-ATA[0][1]*ATA[1][3]*ATA[3][2];
    adjATA[0][3] = ATA[0][3]*ATA[1][2]*ATA[2][1]+ATA[0][2]*ATA[1][1]*ATA[2][3]+ATA[0][1]*ATA[1][3]*ATA[2][2]-ATA[0][1]*ATA[1][2]*ATA[2][3]-ATA[0][2]*ATA[1][3]*ATA[2][1]-ATA[0][3]*ATA[1][1]*ATA[2][2];
    adjATA[1][0] = ATA[1][3]*ATA[2][2]*ATA[3][0]+ATA[1][2]*ATA[2][0]*ATA[3][3]+ATA[1][0]*ATA[2][3]*ATA[3][2]-ATA[1][0]*ATA[2][2]*ATA[3][3]-ATA[1][2]*ATA[2][3]*ATA[3][0]-ATA[1][3]*ATA[2][0]*ATA[3][2];
    adjATA[1][1] = ATA[0][0]*ATA[2][2]*ATA[3][3]+ATA[0][2]*ATA[2][3]*ATA[3][0]+ATA[0][3]*ATA[2][0]*ATA[3][2]-ATA[0][3]*ATA[2][2]*ATA[3][0]-ATA[0][2]*ATA[2][0]*ATA[3][3]-ATA[0][0]*ATA[2][3]*ATA[3][2];
    adjATA[1][2] = ATA[0][3]*ATA[1][2]*ATA[3][0]+ATA[0][2]*ATA[1][0]*ATA[3][3]+ATA[0][0]*ATA[1][3]*ATA[3][2]-ATA[0][0]*ATA[1][2]*ATA[3][3]-ATA[0][2]*ATA[1][3]*ATA[3][0]-ATA[0][3]*ATA[1][0]*ATA[3][2];
    adjATA[1][3] = ATA[0][0]*ATA[1][2]*ATA[2][3]+ATA[0][2]*ATA[1][3]*ATA[2][0]+ATA[0][3]*ATA[1][0]*ATA[2][2]-ATA[0][3]*ATA[1][2]*ATA[2][0]-ATA[0][2]*ATA[1][0]*ATA[2][3]-ATA[0][0]*ATA[1][3]*ATA[2][2];
    adjATA[2][0] = ATA[1][0]*ATA[2][1]*ATA[3][3]+ATA[1][1]*ATA[2][3]*ATA[3][0]+ATA[1][3]*ATA[2][0]*ATA[3][1]-ATA[1][3]*ATA[2][1]*ATA[3][0]-ATA[1][1]*ATA[2][0]*ATA[3][3]-ATA[1][0]*ATA[2][3]*ATA[3][1];
    adjATA[2][1] = ATA[0][3]*ATA[2][1]*ATA[3][0]+ATA[0][1]*ATA[2][0]*ATA[3][3]+ATA[0][0]*ATA[2][3]*ATA[3][1]-ATA[0][0]*ATA[2][1]*ATA[3][3]-ATA[0][1]*ATA[2][3]*ATA[3][0]-ATA[0][3]*ATA[2][0]*ATA[3][1];
    adjATA[2][2] = ATA[0][0]*ATA[1][1]*ATA[3][3]+ATA[0][1]*ATA[1][3]*ATA[3][0]+ATA[0][3]*ATA[1][0]*ATA[3][1]-ATA[0][3]*ATA[1][1]*ATA[3][0]-ATA[0][1]*ATA[1][0]*ATA[3][3]-ATA[0][0]*ATA[1][3]*ATA[3][1];
    adjATA[2][3] = ATA[0][3]*ATA[1][1]*ATA[2][0]+ATA[0][1]*ATA[1][0]*ATA[2][3]+ATA[0][0]*ATA[1][3]*ATA[2][1]-ATA[0][0]*ATA[1][1]*ATA[2][3]-ATA[0][1]*ATA[1][3]*ATA[2][0]-ATA[0][3]*ATA[1][0]*ATA[2][1];
    adjATA[3][0] = ATA[1][2]*ATA[2][1]*ATA[3][0]+ATA[1][1]*ATA[2][0]*ATA[3][2]+ATA[1][0]*ATA[2][2]*ATA[3][1]-ATA[1][0]*ATA[2][1]*ATA[3][2]-ATA[1][1]*ATA[2][2]*ATA[3][0]-ATA[1][2]*ATA[2][0]*ATA[3][1];
    adjATA[3][1] = ATA[0][0]*ATA[2][1]*ATA[3][2]+ATA[0][1]*ATA[2][2]*ATA[3][0]+ATA[0][2]*ATA[2][0]*ATA[3][1]-ATA[0][2]*ATA[2][1]*ATA[3][0]-ATA[0][1]*ATA[2][0]*ATA[3][2]-ATA[0][0]*ATA[2][2]*ATA[3][1];
    adjATA[3][2] = ATA[0][2]*ATA[1][1]*ATA[3][0]+ATA[0][1]*ATA[1][0]*ATA[3][2]+ATA[0][0]*ATA[1][2]*ATA[3][1]-ATA[0][0]*ATA[1][1]*ATA[3][2]-ATA[0][1]*ATA[1][2]*ATA[3][0]-ATA[0][2]*ATA[1][0]*ATA[3][1];
    adjATA[3][3] = ATA[0][0]*ATA[1][1]*ATA[2][2]+ATA[0][1]*ATA[1][2]*ATA[2][0]+ATA[0][2]*ATA[1][0]*ATA[2][1]-ATA[0][2]*ATA[1][1]*ATA[2][0]-ATA[0][1]*ATA[1][0]*ATA[2][2]-ATA[0][0]*ATA[1][2]*ATA[2][1];
    
    std::vector<std::vector<double>> Q;
    for (std::vector<double> vec : adjATA) {
        std::vector<double> row;
        for (double elemenent : vec) {
            row.push_back(elemenent/detATA);
        }
        Q.push_back(row);
    }

    std::vector<std::vector<double>> QR;
    QR.push_back({Q[0][0]*rec.rneu[0][0]+Q[0][1]*rec.rneu[1][0]+Q[0][2]*rec.rneu[2][0],Q[0][0]*rec.rneu[0][1]+Q[0][1]*rec.rneu[1][1]+Q[0][2]*rec.rneu[2][1],Q[0][0]*rec.rneu[0][2]+Q[0][1]*rec.rneu[1][2]+Q[0][2]*rec.rneu[2][2]});
    QR.push_back({Q[1][0]*rec.rneu[0][0]+Q[1][1]*rec.rneu[1][0]+Q[1][2]*rec.rneu[2][0],Q[1][0]*rec.rneu[0][1]+Q[1][1]*rec.rneu[1][1]+Q[1][2]*rec.rneu[2][1],Q[1][0]*rec.rneu[0][2]+Q[1][1]*rec.rneu[1][2]+Q[1][2]*rec.rneu[2][2]});
    QR.push_back({Q[2][0]*rec.rneu[0][0]+Q[2][1]*rec.rneu[1][0]+Q[2][2]*rec.rneu[2][0],Q[2][0]*rec.rneu[0][1]+Q[2][1]*rec.rneu[1][1]+Q[2][2]*rec.rneu[2][1],Q[2][0]*rec.rneu[0][2]+Q[2][1]*rec.rneu[1][2]+Q[2][2]*rec.rneu[2][2]});

    std::vector<std::vector<double>> Qneu;
    Qneu.push_back({rec.rneu[0][0]*QR[0][0]+rec.rneu[1][0]*QR[1][0]+rec.rneu[2][0]*QR[2][0],rec.rneu[0][0]*QR[0][1]+rec.rneu[1][0]*QR[1][1]+rec.rneu[2][0]*QR[2][1],rec.rneu[0][0]*QR[0][2]+rec.rneu[1][0]*QR[1][2]+rec.rneu[2][0]*QR[2][2]});
    Qneu.push_back({rec.rneu[0][1]*QR[0][0]+rec.rneu[1][1]*QR[1][0]+rec.rneu[2][1]*QR[2][0],rec.rneu[0][1]*QR[0][1]+rec.rneu[1][1]*QR[1][1]+rec.rneu[2][1]*QR[2][1],rec.rneu[0][1]*QR[0][2]+rec.rneu[1][1]*QR[1][2]+rec.rneu[2][1]*QR[2][2]});
    Qneu.push_back({rec.rneu[0][2]*QR[0][0]+rec.rneu[1][2]*QR[1][0]+rec.rneu[2][2]*QR[2][0],rec.rneu[0][2]*QR[0][1]+rec.rneu[1][2]*QR[1][1]+rec.rneu[2][2]*QR[2][1],rec.rneu[0][2]*QR[0][2]+rec.rneu[1][2]*QR[1][2]+rec.rneu[2][2]*QR[2][2]});

    dops.GDOP = sqrt(Q[0][0]+Q[1][1]+Q[2][2]+Q[3][3]);
    dops.PDOP = sqrt(Q[0][0]+Q[1][1]+Q[2][2]);
    dops.TDOP = sqrt(Q[3][3]);
    dops.HDOP = sqrt(Qneu[0][0]+Qneu[1][1]);
    dops.VDOP = sqrt(Qneu[2][2]);
    return dops;
}

Plot prepare_plots() {
    Plot plt;

    plt.dop = popen("gnuplot 2>/dev/null","w");
    fprintf(plt.dop,"set term pngcairo size 600,300\n");
    fprintf(plt.dop,"set output 'test.png'\n");
    fprintf(plt.dop,"set key right outside\n");
    fprintf(plt.dop,"set xlabel 'epoka (1 epoka = 60 sekund)'\n");
    fprintf(plt.dop,"set ylabel 'wartosc'\n");
    fprintf(plt.dop,"set grid\n");
    fprintf(plt.dop,"$DATA << EOD\n");

    plt.world = popen("gnuplot 2>/dev/null","w");
    fprintf(plt.world,"set term pngcairo size 500,300\n");
    fprintf(plt.world,"set output 'world.png'\n");
    // fprintf(plt.world,"set key right outside\n");
    fprintf(plt.world,"set xrange[-178:178]\n");
    fprintf(plt.world,"set yrange[-84:90]\n");
    fprintf(plt.world,"set xlabel 'phi [deg]'\n");
    fprintf(plt.world,"set ylabel 'lambda [deg]'\n");
    fprintf(plt.world,"set grid\n");
    fprintf(plt.world,"unset colorbox\n");
    fprintf(plt.world,"set palette model RGB maxcolors 6\n");
    fprintf(plt.world,"set palette defined ( 0 '#57e012', 1 '#e02a12', 2 '#1250e0' , 3 '#e0c512',4 '#6b12e0',5 '#000000')\n");
    fprintf(plt.world,"$DATA << EOD\n");

    plt.skyplot = popen("gnuplot","w");
    fprintf(plt.skyplot,"set term pngcairo size 400,400\n");
    fprintf(plt.skyplot,"set output 'skyplot.png'\n");
    fprintf(plt.skyplot,"unset colorbox\n");
    fprintf(plt.skyplot,"set palette model RGB maxcolors 6\n");
    fprintf(plt.skyplot,"set palette defined ( 0 '#57e012', 1 '#e02a12', 2 '#1250e0' , 3 '#e0c512',4 '#6b12e0',5 '#000000')\n");
    fprintf(plt.skyplot,"$DATA << EOD\n");

    plt.visible = popen("gnuplot 2>/dev/null","w");
    fprintf(plt.visible,"set term pngcairo size 500,300\n");
    fprintf(plt.visible,"set output 'visible.png'\n");
    fprintf(plt.visible,"set xlabel 'epoka (1 epoka = 600 sekund)'\n");
    fprintf(plt.visible,"set ylabel 'liczba widocznych satelitow'\n");
    fprintf(plt.visible,"set grid\n");
    fprintf(plt.visible,"set ytics 1\n");
    fprintf(plt.visible,"$DATA << EOD\n");

    return plt;
}

void close_plots(Plot plt,int sat_count) {
    fprintf(plt.dop,"EOD\n");
    fprintf(plt.dop,"stats $DATA nooutput\n");
    fprintf(plt.dop,"plot $DATA u 1:3 with lines lc rgb 'red' tit 'PDOP',");
    fprintf(plt.dop,"$DATA u 1:4 with lines lc rgb 'blue' tit 'TDOP',");
    fprintf(plt.dop,"$DATA u 1:2 with lines lc rgb 'orange' tit 'GDOP',");
    fprintf(plt.dop,"$DATA u 1:5 with lines lc rgb '#57e012' tit 'HDOP',");
    fprintf(plt.dop,"$DATA u 1:6 with lines lc rgb '#6b12e0' tit 'VDOP',");
    fprintf(plt.dop,"$DATA u 1:3 every ::STATS_records-1::STATS_records-1 with points pt 2 lw 3 lc rgb 'red' notit,");
    fprintf(plt.dop,"$DATA u 1:4 every ::STATS_records-1::STATS_records-1 with points pt 2 lw 3 lc rgb 'blue' notit,");
    fprintf(plt.dop,"$DATA u 1:5 every ::STATS_records-1::STATS_records-1 with points pt 2 lw 3 lc rgb '#57e012' notit,");
    fprintf(plt.dop,"$DATA u 1:6 every ::STATS_records-1::STATS_records-1 with points pt 2 lw 3 lc rgb '#6b12e0' notit,");
    fprintf(plt.dop,"$DATA u 1:2 every ::STATS_records-1::STATS_records-1 with points pt 2 lw 3 lc rgb 'orange' notit\n");

    fprintf(plt.world,"EOD\n");
    fprintf(plt.world,"$COLOR << EOD\n");
    fprintf(plt.world,"-1000 -1000 0\n");
    fprintf(plt.world,"-1000 -1000 1\n");
    fprintf(plt.world,"-1000 -1000 2\n");
    fprintf(plt.world,"-1000 -1000 3\n");
    fprintf(plt.world,"-1000 -1000 4\n");
    fprintf(plt.world,"-1000 -1000 5\n");
    fprintf(plt.world,"EOD\n");
    fprintf(plt.world,"stats $DATA using 1 nooutput\n");
    for (int i = 0;i<sat_count;i++) {
        fprintf(plt.world,"set table $DATA%i\nplot $DATA u 2:1:3 every ::%i::%i with table\nunset table\n",i,i,i);
    }
    fprintf(plt.world,"plot 'data/world.dat' u 1:2 with lines lc rgb 'gray' notit,");
    fprintf(plt.world,"$DATA u 2:1:3 index STATS_blocks-2 with points pt 2 lw 2 palette notit");
    for (int i = 0;i<sat_count;i++) {
        fprintf(plt.world,",$DATA%i u 1:2:3 with lines lw 2 palette notit",i);
    }
    fprintf(plt.world,",$COLOR with points palette notit\n");
    // fprintf(plt.world,"NaN with lines lw 2 lc rgb '#57e012' title 'GPS',");
    // fprintf(plt.world,"NaN with lines lw 2 lc rgb '#e02a12' title 'GLONASS',");
    // fprintf(plt.world,"NaN with lines lw 2 lc rgb '#1250e0' title 'Galileo',");
    // fprintf(plt.world,"NaN with lines lw 2 lc rgb '#e0c512' title 'BeiDou',");
    // fprintf(plt.world,"NaN with lines lw 2 lc rgb '#6b12e0' title 'QZSS',");
    // fprintf(plt.world,"NaN with lines lw 2 lc rgb '#000000' title 'Other'\n");
    fclose(plt.world);

    fprintf(plt.skyplot,"EOD\n");
    fprintf(plt.skyplot,"$COLOR << EOD\n");
    fprintf(plt.skyplot,"-1000 -1000 0\n");
    fprintf(plt.skyplot,"-1000 -1000 1\n");
    fprintf(plt.skyplot,"-1000 -1000 2\n");
    fprintf(plt.skyplot,"-1000 -1000 3\n");
    fprintf(plt.skyplot,"-1000 -1000 4\n");
    fprintf(plt.skyplot,"-1000 -1000 5\n");
    fprintf(plt.skyplot,"EOD\n");
    fprintf(plt.skyplot,"stats $DATA using 1 nooutput\n");
    for (int i = 0;i<sat_count;i++) {
        fprintf(plt.skyplot,"set table $DATA%i\nplot $DATA u 1:2:3 every ::%i::%i with table\nunset table\n",i,i,i);
    }
    fprintf(plt.skyplot,"set polar\n");
    fprintf(plt.skyplot,"set ttics 0,30\n");
    fprintf(plt.skyplot,"set angles degrees\n");
    fprintf(plt.skyplot,"set mttics 4\n");
    fprintf(plt.skyplot,"set grid r polar 60\n");
    fprintf(plt.skyplot,"unset xtics\n");
    fprintf(plt.skyplot,"unset ytics\n");
    fprintf(plt.skyplot,"set size square\n");
    fprintf(plt.skyplot,"unset key\n");
    fprintf(plt.skyplot,"set rrange [90:0]\n");
    fprintf(plt.skyplot,"set border 0\n");
    fprintf(plt.skyplot,"set format r ''\n");
    fprintf(plt.skyplot,"set ttics norotate offset 1\n");
    fprintf(plt.skyplot,"set theta clockwise top\n");
    fprintf(plt.skyplot,"plot $DATA u 1:2:3 index STATS_blocks-2 with points pt 2 lw 2 palette notit");
    for (int i = 0;i<sat_count;i++) {
        fprintf(plt.skyplot,",$DATA%i u 1:2:3 with lines lw 2 palette notit",i);
    }
    fprintf(plt.skyplot,",$COLOR with points palette notit\n");
    fclose(plt.skyplot);

    fprintf(plt.visible,"EOD\n");
    // fprintf(plt.visible,"set autoscale xfix\n");
    fprintf(plt.visible,"stats $DATA using 2 nooutput\n");
    fprintf(plt.visible,"set yrange [STATS_min-0.2:STATS_max+0.2]\n");
    fprintf(plt.visible,"plot $DATA u 1:2 with lines lw 1 lc rgb 'black' notit\n");
    fclose(plt.visible);
};