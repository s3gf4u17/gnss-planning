struct Satellite {
    double sid; // satellite id
    double health; // health indicator
    double e; // eccentricity
    double sqrta; // square root of semimajor axis
    double Omega; // right ascension of ascending node
    double omega; // argument of perigee
    double anomaly; // mean anomaly
    double toa; // time og almanac in GPS seconds
    double i; // inclination offset to 54deg
    double OmegaDT; // rate of right ascension
    double sco; // satellite clock offset
    double scd; // satellite clock drift
    double week; // GPS week
};

struct Arguments {
    int time0; // observation start
    int time1; // observation end
    int timei; // observation interval
    double phi; // observation place (phi) [radians]
    double lambda; // observation place (lambda) [radians]
    double h; // observation place (lambda)
    double mask; // observation mask [radians]
    std::vector<int> sids;// if -- only specified satellites
};

struct Position {
    float X;
    float Y;
    float Z;
    float phi;
    float lambda;
    float height;
    float azimuth;
    float elevation;
    bool visible;
    int system;
};

struct Receiver {
    float X;
    float Y;
    float Z;
    float phi;
    float lambda;
    float height;
    float mask;
    float rneu[3][3];
};

struct DOPS {
    bool sat4;
    float GDOP;
    float PDOP;
    float HDOP;
    float VDOP;
    float TDOP;
};

struct Plot {
    FILE *skyplot;
    FILE *world;
    FILE *dop;
    FILE *visible;
};