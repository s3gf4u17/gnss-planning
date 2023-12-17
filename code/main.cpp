#include <math.h>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <algorithm>

#define MI 3.986005*pow(10,14)
#define OMEGA_E 7.2921151467*pow(10,-5)

#include "models/models.cpp"
#include "lib/lib.cpp"

int main(int argc, char *argv[]) {
    // parse arguments given via command line
    Arguments args = parse_args(argc,argv);
    // parse almanac data from input file specified by user
    std::vector<Satellite> alm = parse_almanac(argv[1]);
    Receiver rec = locate_receiver(args.phi,args.lambda,args.h,args.mask); // phi lambda h mask

    Plot plt = prepare_plots();

    int epoch = 0;
    for (args.time0;args.time0<=args.time1;args.time0+=args.timei) {
        std::vector<std::vector<double>> A;
        for (Satellite sat : alm) {
            if (!std::count(args.sids.begin(), args.sids.end(), (int)sat.sid)&&args.sids.size()>0) continue;
            Position pos = get_position(sat,rec,args.time0);
            if (pos.visible) A.push_back(genArow(pos,rec));
            fprintf(plt.world,"%f %f %i\n",pos.phi,pos.lambda,pos.system);
            fprintf(plt.skyplot,"%f %f %i\n",pos.azimuth,pos.elevation,pos.system);
        }
        fprintf(plt.world,"\n\n");
        fprintf(plt.skyplot,"\n\n");
        fprintf(plt.visible,"%i %i\n",epoch,A.size());
        DOPS dops = get_DOP(A,rec);
        dops.sat4 ? fprintf(plt.dop,"%i %f %f %f %f %f\n",epoch,dops.GDOP,dops.PDOP,dops.TDOP,dops.HDOP,dops.VDOP) : fprintf(plt.dop,"\n\n");
        epoch++;
    }

    close_plots(plt,(args.sids.size()==0) ? alm.size() : args.sids.size());

    return 0;
}