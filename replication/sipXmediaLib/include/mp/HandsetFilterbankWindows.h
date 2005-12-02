//
// Copyright (C) 2005 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
////////////////////////////////////////////////////////////////////////
//////

/*
 0.01000   1.00000   0.10000
iflat =  8
static double weighting_template[128] = {
 0.01060, 0.01656, 0.02355, 0.03165, 0.04090, 0.05136, 0.06307, 0.07606, 0.09035, 0.10596,
 0.12289, 0.14112, 0.16065, 0.18144, 0.20344, 0.22659, 0.25085, 0.27611, 0.30231, 0.32933,
 0.35708, 0.38544, 0.41429, 0.44351, 0.47296, 0.50251, 0.53203, 0.56139, 0.59045, 0.61908,
 0.64715, 0.67456, 0.70118, 0.72690, 0.75164, 0.77531, 0.79784, 0.81915, 0.83921, 0.85797,
 0.87540, 0.89151, 0.90628, 0.91973, 0.93189, 0.94279, 0.95248, 0.96100, 0.96844, 0.97485,
 0.98031, 0.98491, 0.98872, 0.99183, 0.99433, 0.99629, 0.99781, 0.99895, 0.99978, 1.00037,
 1.00077, 1.00104, 1.00120, 1.00129, 1.00131, 1.00129, 1.00120, 1.00104, 1.00077, 1.00037,
 0.99978, 0.99895, 0.99781, 0.99629, 0.99433, 0.99183, 0.98872, 0.98491, 0.98031, 0.97485,
 0.96844, 0.96100, 0.95248, 0.94279, 0.93189, 0.91973, 0.90628, 0.89151, 0.87540, 0.85797,
 0.83921, 0.81915, 0.79784, 0.77531, 0.75164, 0.72690, 0.70118, 0.67456, 0.64715, 0.61908,
 0.59045, 0.56139, 0.53203, 0.50251, 0.47296, 0.44351, 0.41429, 0.38544, 0.35708, 0.32933,
 0.30231, 0.27611, 0.25085, 0.22659, 0.20344, 0.18144, 0.16065, 0.14112, 0.12289, 0.10596,
 0.09035, 0.07606, 0.06307, 0.05136, 0.04090, 0.03165, 0.02355, 0.01656};
static double ldspkr_weighting_pow[128] = {
 1.00263, 1.00262, 1.00258, 1.00251, 1.00240, 1.00226, 1.00208, 1.00184, 1.00155, 1.00119,
 1.00074, 1.00020, 0.99956, 0.99879, 0.99789, 0.99684, 0.99562, 0.99421, 0.99260, 0.99076,
 0.98868, 0.98635, 0.98372, 0.98080, 0.97756, 0.97398, 0.97004, 0.96572, 0.96101, 0.95588,
 0.95033, 0.94433, 0.93787, 0.93094, 0.92353, 0.91562, 0.90721, 0.89829, 0.88886, 0.87890,
 0.86842, 0.85743, 0.84591, 0.83388, 0.82135, 0.80831, 0.79479, 0.78079, 0.76633, 0.75143,
 0.73611, 0.72038, 0.70427, 0.68780, 0.67101, 0.65391, 0.63654, 0.61893, 0.60111, 0.58311,
 0.56497, 0.54672, 0.52839, 0.51002, 0.49165, 0.47331, 0.45503, 0.43685, 0.41881, 0.40093,
 0.38326, 0.36581, 0.34863, 0.33173, 0.31516, 0.29892, 0.28306, 0.26758, 0.25252, 0.23788,
 0.22369, 0.20996, 0.19670, 0.18392, 0.17164, 0.15985, 0.14856, 0.13778, 0.12751, 0.11773,
 0.10846, 0.09968, 0.09139, 0.08358, 0.07624, 0.06936, 0.06292, 0.05692, 0.05135, 0.04617,
 0.04139, 0.03697, 0.03292, 0.02920, 0.02581, 0.02272, 0.01992, 0.01738, 0.01510, 0.01305,
 0.01123, 0.00960, 0.00816, 0.00690, 0.00578, 0.00482, 0.00398, 0.00326, 0.00264, 0.00211,
 0.00167, 0.00131, 0.00100, 0.00075, 0.00055, 0.00040, 0.00027, 0.00018};
  0     0.00    0.01    0.01  -55.14    0.01     -58.39  -58.39  -71.19  -71.19
  1             0.01    0.01  -55.14    0.01     -58.05  -58.81  -69.87  -72.82
  2     0.00    0.01    0.01  -55.17    0.01     -57.77  -59.31  -68.76  -74.88
  3             0.01    0.01  -55.22    0.01     -57.57  -59.90  -67.83  -77.67
  4     0.00    0.01    0.01  -55.29    0.01     -57.43  -60.59  -67.04  -81.87
  5             0.01    0.01  -55.37    0.01     -57.35  -61.41  -66.36  -90.43
  6     0.00    0.01    0.01  -55.47    0.01     -57.33  -62.37  -65.78  -93.75
  7             0.01    0.01  -55.59    0.01     -57.38  -63.51  -65.30  -82.99
  8     0.00    0.01    0.01  -55.73    0.01     -57.50  -64.89  -64.88  -78.37
  9             0.01    0.01  -55.87    0.01     -57.68  -66.57  -64.54  -75.41
 10     0.00    0.01    0.00  -56.03    0.00     -57.93  -68.73  -64.27  -73.26
 11             0.01    0.00  -56.20    0.00     -58.26  -71.67  -64.06  -71.59
 12     0.00    0.02   -0.00  -56.37   -0.00     -58.67  -76.18  -63.91  -70.23
 13             0.02   -0.01  -56.54   -0.01     -59.18  -86.18  -63.81  -69.12
 14     0.00    0.02   -0.01  -56.71   -0.01     -59.79  -84.96  -63.77  -68.18
 15             0.02   -0.01  -56.87   -0.01     -60.53  -75.87  -63.79  -67.38
 16     0.00    0.02   -0.02  -57.02   -0.02     -61.42  -71.59  -63.87  -66.71
 17             0.02   -0.03  -57.14   -0.03     -62.51  -68.80  -64.00  -66.13
 18    -0.00    0.02   -0.03  -57.23   -0.03     -63.83  -66.76  -64.20  -65.65
 19             0.02   -0.04  -57.29   -0.04     -65.50  -65.16  -64.46  -65.25
 20    -0.02    0.02   -0.05  -57.30   -0.05     -67.68  -63.88  -64.79  -64.92
 21             0.02   -0.06  -57.28   -0.06     -70.75  -62.82  -65.19  -64.65
 22    -0.04    0.02   -0.07  -57.21   -0.07     -75.74  -61.94  -65.68  -64.45
 23             0.02   -0.08  -57.10   -0.08     -89.47  -61.20  -66.26  -64.31
 24    -0.07    0.01   -0.10  -56.95   -0.10     -80.14  -60.58  -66.94  -64.23
 25             0.01   -0.11  -56.76   -0.11     -72.71  -60.06  -67.75  -64.21
 26    -0.12    0.01   -0.13  -56.55   -0.13     -68.75  -59.64  -68.72  -64.24
 27             0.01   -0.15  -56.32   -0.15     -66.06  -59.29  -69.88  -64.33
 28    -0.17    0.01   -0.17  -56.08   -0.17     -64.03  -59.02  -71.29  -64.48
 29             0.01   -0.20  -55.83   -0.20     -62.42  -58.82  -73.06  -64.69
 30    -0.23    0.01   -0.22  -55.58   -0.22     -61.10  -58.68  -75.39  -64.97
 31             0.01   -0.25  -55.33   -0.25     -59.99  -58.61  -78.68  -65.31
 32    -0.30    0.00   -0.28  -55.10   -0.28     -59.05  -58.60  -84.22  -65.73
 33             0.00   -0.31  -54.89   -0.31     -58.26  -58.65 -104.76  -66.23
 34    -0.38   -0.00   -0.35  -54.69   -0.35     -57.58  -58.77  -85.95  -66.83
 35            -0.00   -0.38  -54.52   -0.38     -57.00  -58.95  -79.43  -67.53
 36    -0.47   -0.01   -0.42  -54.37   -0.42     -56.51  -59.19  -75.77  -68.36
 37            -0.01   -0.47  -54.24   -0.47     -56.11  -59.51  -73.22  -69.35
 38    -0.58   -0.01   -0.51  -54.15   -0.51     -55.78  -59.90  -71.28  -70.53
 39            -0.01   -0.56  -54.09   -0.56     -55.53  -60.38  -69.73  -71.98
 40    -0.69   -0.02   -0.61  -54.05   -0.61     -55.34  -60.94  -68.46  -73.79
 41            -0.02   -0.67  -54.05   -0.67     -55.23  -61.61  -67.39  -76.17
 42    -0.81   -0.02   -0.73  -54.09   -0.73     -55.18  -62.41  -66.48  -79.56
 43            -0.03   -0.79  -54.16   -0.79     -55.21  -63.35  -65.70  -85.34
 44    -0.95   -0.03   -0.85  -54.26   -0.85     -55.30  -64.48  -65.03 -112.94
 45            -0.03   -0.92  -54.40   -0.92     -55.48  -65.85  -64.46  -86.04
 46    -1.09   -0.04   -1.00  -54.57   -1.00     -55.73  -67.56  -63.97  -79.83
 47            -0.04   -1.07  -54.78   -1.07     -56.07  -69.76  -63.56  -76.26
 48    -1.25   -0.04   -1.16  -55.02   -1.16     -56.50  -72.82  -63.22  -73.77
 49            -0.05   -1.24  -55.28   -1.24     -57.05  -77.69  -62.95  -71.87
 50    -1.42   -0.05   -1.33  -55.56   -1.33     -57.73  -90.13  -62.73  -70.35
 51            -0.05   -1.42  -55.85   -1.42     -58.56  -83.27  -62.58  -69.10
 52    -1.60   -0.06   -1.52  -56.14   -1.52     -59.59  -75.48  -62.49  -68.05
 53            -0.06   -1.63  -56.40   -1.63     -60.88  -71.47  -62.45  -67.15
 54    -1.80   -0.06   -1.73  -56.62   -1.73     -62.52  -68.77  -62.47  -66.39
 55            -0.06   -1.84  -56.77   -1.84     -64.71  -66.76  -62.55  -65.73
 56    -2.01   -0.06   -1.96  -56.83   -1.96     -67.83  -65.17  -62.69  -65.17
 57            -0.07   -2.08  -56.77   -2.08     -73.10  -63.87  -62.90  -64.70
 58    -2.24   -0.07   -2.21  -56.60   -2.21     -90.52  -62.79  -63.18  -64.29
 59            -0.07   -2.34  -56.32   -2.34     -75.51  -61.88  -63.53  -63.96
 60    -2.48   -0.07   -2.48  -55.93   -2.48     -68.64  -61.11  -63.96  -63.69
 61            -0.07   -2.62  -55.48   -2.62     -64.81  -60.45  -64.48  -63.49
 62    -2.74   -0.07   -2.77  -54.98   -2.77     -62.15  -59.89  -65.12  -63.34
 63            -0.07   -2.92  -54.45   -2.92     -60.11  -59.42  -65.87  -63.25
 64    -3.01   -0.07   -3.08  -53.93   -3.08     -58.48  -59.03  -66.78  -63.21
 65            -0.07   -3.25  -53.41   -3.25     -57.13  -58.71  -67.89  -63.24
 66    -3.30   -0.07   -3.42  -52.91   -3.42     -55.99  -58.45  -69.24  -63.32
 67            -0.07   -3.60  -52.45   -3.60     -55.02  -58.27  -70.96  -63.47
 68    -3.62   -0.07   -3.78  -52.02   -3.78     -54.19  -58.14  -73.22  -63.67
 69            -0.07   -3.97  -51.64   -3.97     -53.48  -58.08  -76.45  -63.95
 70    -3.95   -0.07   -4.17  -51.30   -4.17     -52.87  -58.07  -81.89  -64.30
 71            -0.07   -4.37  -51.00   -4.37     -52.36  -58.13 -101.61  -64.73
 72    -4.31   -0.06   -4.58  -50.76   -4.58     -51.93  -58.25  -83.68  -65.25
 73            -0.06   -4.79  -50.56   -4.79     -51.59  -58.43  -77.05  -65.88
 74    -4.69   -0.06   -5.01  -50.41   -5.01     -51.32  -58.69  -73.31  -66.63
 75            -0.06   -5.24  -50.32   -5.24     -51.13  -59.01  -70.69  -67.53
 76    -5.11   -0.06   -5.48  -50.29   -5.48     -51.02  -59.43  -68.68  -68.63
 77            -0.05   -5.73  -50.31   -5.73     -50.99  -59.93  -67.07  -69.99
 78    -5.55   -0.05   -5.98  -50.39   -5.98     -51.03  -60.53  -65.72  -71.70
 79            -0.05   -6.24  -50.54   -6.24     -51.17  -61.27  -64.58  -73.96
 80    -6.02   -0.04   -6.50  -50.76   -6.50     -51.40  -62.15  -63.60  -77.20
 81            -0.04   -6.78  -51.05   -6.78     -51.74  -63.21  -62.75  -82.73
 82    -6.53   -0.04   -7.06  -51.41   -7.06     -52.19  -64.53  -62.00 -104.44
 83            -0.03   -7.35  -51.86   -7.35     -52.80  -66.19  -61.35  -84.03
 84    -7.09   -0.03   -7.65  -52.37   -7.65     -53.57  -68.38  -60.78  -77.50
 85            -0.03   -7.96  -52.96   -7.96     -54.56  -71.47  -60.28  -73.77
 86    -7.69   -0.02   -8.28  -53.60   -8.28     -55.86  -76.59  -59.85  -71.14
 87            -0.02   -8.61  -54.23   -8.61     -57.58  -92.02  -59.48  -69.11
 88    -8.34   -0.02   -8.94  -54.77   -8.94     -59.98  -79.87  -59.17  -67.47
 89            -0.01   -9.29  -55.09   -9.29     -63.70  -72.73  -58.91  -66.09
 90    -9.06   -0.01   -9.65  -55.05   -9.65     -71.17  -68.81  -58.70  -64.91
 91            -0.01  -10.01  -54.59  -10.01     -77.92  -66.09  -58.54  -63.88
 92    -9.86   -0.01  -10.39  -53.75  -10.39     -65.11  -64.01  -58.44  -62.98
 93            -0.00  -10.78  -52.69  -10.78     -59.95  -62.33  -58.38  -62.18
 94   -10.74   -0.00  -11.18  -51.52  -11.18     -56.62  -60.93  -58.38  -61.47
 95             0.00  -11.59  -50.35  -11.59     -54.14  -59.73  -58.42  -60.84
 96   -11.74    0.00  -12.01  -49.23  -12.01     -52.16  -58.68  -58.52  -60.27
 97             0.01  -12.45  -48.17  -12.45     -50.53  -57.77  -58.68  -59.77
 98   -12.88    0.01  -12.89  -47.19  -12.89     -49.13  -56.96  -58.89  -59.31
 99             0.01  -13.36  -46.29  -13.35     -47.93  -56.24  -59.17  -58.91
100   -14.20    0.01  -13.83  -45.47  -13.83     -46.88  -55.59  -59.51  -58.55
101             0.01  -14.32  -44.73  -14.32     -45.95  -55.02  -59.94  -58.24
102   -15.76    0.01  -14.83  -44.05  -14.82     -45.13  -54.50  -60.45  -57.96
103             0.01  -15.35  -43.45  -15.34     -44.41  -54.04  -61.06  -57.72
104   -17.69    0.01  -15.88  -42.91  -15.87     -43.78  -53.63  -61.78  -57.52
105             0.02  -16.44  -42.43  -16.43     -43.23  -53.26  -62.66  -57.36
106   -20.17    0.02  -17.01  -42.02  -16.99     -42.76  -52.94  -63.71  -57.23
107             0.02  -17.60  -41.67  -17.58     -42.37  -52.66  -65.00  -57.13
108   -23.69    0.02  -18.21  -41.39  -18.19     -42.05  -52.41  -66.62  -57.07
109             0.02  -18.84  -41.17  -18.82     -41.80  -52.20  -68.74  -57.04
110   -29.70    0.02  -19.50  -41.02  -19.47     -41.63  -52.02  -71.72  -57.04
111             0.02  -20.18  -40.94  -20.14     -41.54  -51.88  -76.55  -57.08
112    -1.#J    0.02  -20.88  -40.95  -20.84     -41.54  -51.77  -89.32  -57.14
113             0.02  -21.61  -41.03  -21.56     -41.64  -51.69  -81.52  -57.24
114    -1.#J    0.02  -22.38  -41.22  -22.32     -41.85  -51.65  -73.76  -57.38
115             0.02  -23.17  -41.52  -23.11     -42.18  -51.63  -69.66  -57.55
116    -1.#J    0.02  -24.00  -41.95  -23.93     -42.67  -51.64  -66.84  -57.76
117             0.01  -24.87  -42.53  -24.80     -43.35  -51.69  -64.70  -58.00
118    -1.#J    0.01  -25.79  -43.31  -25.71     -44.29  -51.76  -62.97  -58.29
119             0.01  -26.75  -44.33  -26.67     -45.58  -51.87  -61.53  -58.61
120    -1.#J    0.01  -27.76  -45.66  -27.69     -47.42  -52.01  -60.30  -58.99
121             0.01  -28.84  -47.36  -28.78     -50.20  -52.18  -59.22  -59.41
122    -1.#J    0.01  -29.99  -49.32  -29.94     -55.18  -52.38  -58.28  -59.88
123             0.01  -31.23  -50.68  -31.18     -73.62  -52.62  -57.44  -60.42
124    -1.#J    0.01  -32.56  -49.72  -32.48     -56.35  -52.89  -56.69  -61.02
125             0.01  -34.01  -46.94  -33.80     -49.22  -53.20  -56.02  -61.70
126    -1.#J    0.01  -35.62  -44.02  -35.03     -45.02  -53.55  -55.42  -62.46
127             0.01  -37.42  -41.43  -35.97     -41.95  -53.95  -54.88  -63.33
128    -1.#J    0.01  -39.49  -39.19  -36.33     -39.49  -54.39  -54.39  -64.33
in band to aliased ratio =   45.11425
in band to distant aliased ratio =   55.99247
overlapped passband max deviation from flat =    0.08974
in band to weighted aliased ratio =   54.55673
analysis filter reconstruction errors by phase:
phase =  0/  8   0.9998   0.0046   0.0012 
phase =  1/  8   0.9999   0.0047  -0.0012 
phase =  2/  8   1.0001   0.0050  -0.0033 
phase =  3/  8   1.0003   0.0053  -0.0045 
phase =  4/  8   1.0003   0.0053  -0.0045 
phase =  5/  8   1.0001   0.0050  -0.0033 
phase =  6/  8   0.9999   0.0047  -0.0012 
phase =  7/  8   0.9998   0.0046   0.0012 
*/
static int AnalysisWindow[ 48] = {
  -350,  -630,  -963, -1330, -1700, -2030, -2266, -2353, -2044, -1468,
  -530,   813,  2575,  4751,  7297, 10154, 13103, 16119, 19074, 21817,
 24209, 26117, 27438, 28087, 28087, 27438, 26117, 24209, 21817, 19074,
 16119, 13103, 10154,  7297,  4751,  2575,   813,  -530, -1468, -2044,
 -2353, -2266, -2030, -1700, -1330,  -963,  -630,  -350};


/*
 0.02000   1.00000   0.10000
iflat =  0
  0     0.00   -0.22   -0.23  -72.45   -0.23     -76.63  -76.63  -82.17  -82.17
  1            -0.21   -0.23  -70.95   -0.23     -87.20  -72.10  -86.77  -79.23
  2    -0.00   -0.21   -0.23  -68.24   -0.23     -84.14  -69.22  -97.46  -77.09
  3            -0.21   -0.23  -65.78   -0.23     -75.43  -67.13  -94.29  -75.42
  4    -0.01   -0.21   -0.23  -63.76   -0.23     -71.14  -65.52  -85.67  -74.08
  5            -0.20   -0.23  -62.10   -0.23     -68.29  -64.23  -81.44  -72.96
  6    -0.02   -0.20   -0.23  -60.73   -0.23     -66.15  -63.18  -78.64  -72.03
  7            -0.20   -0.24  -59.57   -0.24     -64.45  -62.30  -76.55  -71.23
  8    -0.04   -0.20   -0.24  -58.58   -0.24     -63.06  -61.57  -74.91  -70.56
  9            -0.19   -0.24  -57.73   -0.24     -61.89  -60.95  -73.57  -69.98
 10    -0.07   -0.19   -0.25  -57.00   -0.25     -60.89  -60.44  -72.44  -69.50
 11            -0.19   -0.25  -56.37   -0.25     -60.04  -60.01  -71.49  -69.09
 12    -0.09   -0.18   -0.26  -55.82   -0.26     -59.29  -59.67  -70.67  -68.75
 13            -0.18   -0.26  -55.35   -0.26     -58.65  -59.39  -69.97  -68.48
 14    -0.13   -0.17   -0.27  -54.95   -0.27     -58.09  -59.19  -69.37  -68.27
 15            -0.17   -0.28  -54.62   -0.28     -57.62  -59.04  -68.86  -68.12
 16    -0.17   -0.16   -0.28  -54.35   -0.28     -57.21  -58.95  -68.42  -68.02
 17            -0.16   -0.29  -54.13   -0.29     -56.87  -58.92  -68.05  -67.97
 18    -0.21   -0.15   -0.30  -53.97   -0.30     -56.59  -58.95  -67.74  -67.98
 19            -0.15   -0.31  -53.87   -0.31     -56.38  -59.03  -67.50  -68.05
 20    -0.26   -0.14   -0.32  -53.81   -0.32     -56.22  -59.17  -67.31  -68.16
 21            -0.13   -0.34  -53.81   -0.34     -56.12  -59.37  -67.18  -68.34
 22    -0.32   -0.13   -0.35  -53.87   -0.35     -56.08  -59.63  -67.10  -68.57
 23            -0.12   -0.36  -53.97   -0.36     -56.10  -59.94  -67.08  -68.86
 24    -0.38   -0.11   -0.38  -54.14   -0.38     -56.18  -60.33  -67.11  -69.22
 25            -0.10   -0.40  -54.37   -0.40     -56.32  -60.78  -67.19  -69.65
 26    -0.45   -0.09   -0.41  -54.66   -0.41     -56.53  -61.32  -67.34  -70.15
 27            -0.09   -0.43  -55.02   -0.43     -56.81  -61.94  -67.54  -70.75
 28    -0.52   -0.08   -0.46  -55.46   -0.46     -57.17  -62.66  -67.80  -71.44
 29            -0.07   -0.48  -55.98   -0.48     -57.62  -63.50  -68.13  -72.26
 30    -0.60   -0.06   -0.50  -56.61   -0.50     -58.17  -64.49  -68.53  -73.22
 31            -0.05   -0.53  -57.36   -0.53     -58.84  -65.65  -69.02  -74.35
 32    -0.69   -0.04   -0.56  -58.25   -0.56     -59.65  -67.04  -69.60  -75.72
 33            -0.03   -0.59  -59.32   -0.59     -60.64  -68.75  -70.28  -77.42
 34    -0.78   -0.02   -0.62  -60.61   -0.62     -61.86  -70.93  -71.09  -79.60
 35            -0.01   -0.65  -62.22   -0.65     -63.39  -73.89  -72.05  -82.58
 36    -0.88   -0.00   -0.69  -64.26   -0.69     -65.39  -78.47  -73.21  -87.27
 37             0.01   -0.72  -66.96   -0.72     -68.16  -88.77  -74.63  -98.40
 38    -0.98    0.02   -0.76  -70.57   -0.76     -72.51  -86.75  -76.41  -94.26
 39             0.03   -0.80  -73.93   -0.80     -82.30  -77.89  -78.76  -85.90
 40    -1.09    0.04   -0.85  -71.85   -0.85     -81.05  -73.66  -82.12  -81.75
 41             0.05   -0.90  -67.79   -0.90     -71.76  -70.88  -87.86  -78.99
 42    -1.21    0.06   -0.95  -64.61   -0.95     -67.31  -68.83 -114.84  -76.94
 43             0.07   -1.00  -62.19   -1.00     -64.35  -67.24  -88.56  -75.32
 44    -1.33    0.08   -1.05  -60.27   -1.05     -62.13  -65.94  -82.30  -74.00
 45             0.09   -1.11  -58.70   -1.11     -60.37  -64.87  -78.69  -72.90
 46    -1.46    0.10   -1.17  -57.38   -1.17     -58.91  -63.97  -76.16  -71.96
 47             0.11   -1.23  -56.25   -1.23     -57.69  -63.21  -74.23  -71.17
 48    -1.60    0.12   -1.30  -55.28   -1.30     -56.63  -62.57  -72.67  -70.49
 49             0.12   -1.37  -54.44   -1.37     -55.72  -62.02  -71.38  -69.90
 50    -1.75    0.13   -1.44  -53.71   -1.44     -54.93  -61.56  -70.29  -69.41
 51             0.14   -1.52  -53.07   -1.52     -54.24  -61.18  -69.35  -68.98
 52    -1.90    0.15   -1.59  -52.52   -1.59     -53.64  -60.87  -68.55  -68.63
 53             0.15   -1.68  -52.04   -1.68     -53.12  -60.62  -67.85  -68.34
 54    -2.07    0.16   -1.76  -51.63   -1.76     -52.67  -60.44  -67.25  -68.12
 55             0.16   -1.85  -51.29   -1.85     -52.30  -60.31  -66.72  -67.95
 56    -2.24    0.17   -1.94  -51.01   -1.94     -51.99  -60.24  -66.27  -67.83
 57             0.17   -2.04  -50.79   -2.04     -51.74  -60.23  -65.89  -67.77
 58    -2.42    0.18   -2.14  -50.63   -2.14     -51.56  -60.27  -65.57  -67.76
 59             0.18   -2.24  -50.53   -2.24     -51.43  -60.37  -65.31  -67.81
 60    -2.60    0.18   -2.35  -50.49   -2.35     -51.38  -60.52  -65.11  -67.92
 61             0.19   -2.46  -50.52   -2.46     -51.39  -60.74  -64.96  -68.08
 62    -2.80    0.19   -2.58  -50.61   -2.58     -51.46  -61.02  -64.86  -68.30
 63             0.19   -2.70  -50.77   -2.70     -51.61  -61.36  -64.82  -68.58
 64    -3.01    0.19   -2.82  -51.01   -2.82     -51.85  -61.77  -64.84  -68.94
 65             0.19   -2.95  -51.33   -2.95     -52.17  -62.27  -64.91  -69.36
 66    -3.23    0.19   -3.08  -51.75   -3.08     -52.59  -62.86  -65.03  -69.88
 67             0.19   -3.22  -52.28   -3.22     -53.13  -63.55  -65.22  -70.49
 68    -3.46    0.18   -3.36  -52.93   -3.36     -53.80  -64.36  -65.47  -71.21
 69             0.18   -3.51  -53.75   -3.51     -54.66  -65.32  -65.79  -72.07
 70    -3.70    0.18   -3.66  -54.76   -3.66     -55.73  -66.47  -66.19  -73.09
 71             0.17   -3.81  -56.04   -3.81     -57.11  -67.87  -66.68  -74.33
 72    -3.95    0.17   -3.97  -57.67   -3.97     -58.92  -69.62  -67.27  -75.87
 73             0.16   -4.14  -59.84   -4.14     -61.46  -71.90  -67.98  -77.83
 74    -4.22    0.16   -4.31  -62.85   -4.31     -65.43  -75.10  -68.83  -80.48
 75             0.15   -4.48  -66.99   -4.48     -73.81  -80.40  -69.85  -84.48
 76    -4.50    0.15   -4.67  -68.97   -4.67     -76.60  -97.07  -71.12  -92.45
 77             0.14   -4.85  -64.66   -4.85     -65.70  -83.30  -72.70  -97.87
 78    -4.79    0.13   -5.04  -60.52   -5.04     -60.85  -76.40  -74.77  -86.03
 79             0.12   -5.24  -57.44   -5.24     -57.65  -72.61  -77.67  -81.15
 80    -5.11    0.12   -5.44  -55.06   -5.44     -55.24  -69.98  -82.30  -78.04
 81             0.11   -5.65  -53.14   -5.65     -53.32  -67.99  -93.72  -75.75
 82    -5.43    0.10   -5.87  -51.53   -5.87     -51.71  -66.39  -88.55  -73.95
 83             0.09   -6.09  -50.15   -6.09     -50.34  -65.07  -80.27  -72.47
 84    -5.78    0.08   -6.31  -48.95   -6.31     -49.15  -63.95  -76.04  -71.23
 85             0.07   -6.54  -47.90   -6.54     -48.10  -62.99  -73.16  -70.16
 86    -6.14    0.06   -6.78  -46.97   -6.78     -47.17  -62.16  -70.98  -69.24
 87             0.05   -7.03  -46.14   -7.03     -46.35  -61.43  -69.23  -68.43
 88    -6.53    0.04   -7.28  -45.40   -7.28     -45.61  -60.80  -67.77  -67.72
 89             0.03   -7.54  -44.74   -7.54     -44.96  -60.24  -66.52  -67.09
 90    -6.94    0.02   -7.80  -44.16   -7.80     -44.38  -59.75  -65.44  -66.53
 91             0.01   -8.08  -43.64   -8.08     -43.86  -59.33  -64.49  -66.04
 92    -7.38   -0.00   -8.36  -43.19   -8.36     -43.42  -58.96  -63.65  -65.61
 93            -0.01   -8.64  -42.81   -8.64     -43.03  -58.64  -62.90  -65.23
 94    -7.85   -0.02   -8.94  -42.48   -8.94     -42.71  -58.37  -62.23  -64.90
 95            -0.03   -9.24  -42.22   -9.24     -42.45  -58.15  -61.62  -64.62
 96    -8.34   -0.04   -9.55  -42.02   -9.55     -42.26  -57.97  -61.08  -64.38
 97            -0.05   -9.87  -41.88   -9.87     -42.13  -57.83  -60.60  -64.19
 98    -8.88   -0.06  -10.20  -41.81  -10.19     -42.07  -57.74  -60.17  -64.03
 99            -0.07  -10.53  -41.81  -10.53     -42.08  -57.68  -59.78  -63.91
100    -9.45   -0.08  -10.87  -41.90  -10.87     -42.18  -57.66  -59.44  -63.84
101            -0.09  -11.23  -42.06  -11.22     -42.37  -57.69  -59.14  -63.80
102   -10.07   -0.09  -11.59  -42.33  -11.58     -42.67  -57.75  -58.88  -63.80
103            -0.10  -11.96  -42.72  -11.96     -43.09  -57.85  -58.66  -63.84
104   -10.74   -0.11  -12.34  -43.24  -12.34     -43.66  -57.99  -58.48  -63.91
105            -0.12  -12.73  -43.93  -12.73     -44.42  -58.18  -58.33  -64.03
106   -11.48   -0.13  -13.13  -44.84  -13.13     -45.45  -58.41  -58.22  -64.18
107            -0.13  -13.55  -46.03  -13.54     -46.83  -58.68  -58.14  -64.38
108   -12.29   -0.14  -13.97  -47.61  -13.97     -48.78  -59.01  -58.10  -64.63
109            -0.15  -14.40  -49.72  -14.40     -51.73  -59.39  -58.09  -64.92
110   -13.19   -0.15  -14.85  -52.41  -14.85     -57.07  -59.82  -58.12  -65.26
111            -0.16  -15.31  -54.47  -15.31     -83.88  -60.32  -58.18  -65.66
112   -14.20   -0.16  -15.78  -52.68  -15.78     -56.90  -60.89  -58.28  -66.12
113            -0.17  -16.27  -48.99  -16.27     -50.22  -61.54  -58.42  -66.64
114   -15.34   -0.17  -16.77  -45.70  -16.76     -46.19  -62.29  -58.60  -67.24
115            -0.18  -17.28  -42.99  -17.27     -43.22  -63.14  -58.82  -67.93
116   -16.67   -0.18  -17.81  -40.72  -17.79     -40.84  -64.13  -59.08  -68.72
117            -0.19  -18.36  -38.76  -18.32     -38.83  -65.29  -59.39  -69.64
118   -18.24   -0.19  -18.92  -37.04  -18.86     -37.08  -66.67  -59.76  -70.71
119            -0.19  -19.51  -35.49  -19.40     -35.52  -68.35  -60.18  -71.97
120   -20.17   -0.20  -20.11  -34.09  -19.94     -34.10  -70.48  -60.66  -73.50
121            -0.20  -20.73  -32.80  -20.47     -32.81  -73.35  -61.21  -75.40
122   -22.67   -0.20  -21.37  -31.60  -20.98     -31.61  -77.72  -61.84  -77.89
123            -0.20  -22.04  -30.48  -21.46     -30.49  -87.00  -62.57  -81.47
124   -26.18   -0.21  -22.73  -29.44  -21.89     -29.44  -87.83  -63.41  -87.73
125            -0.21  -23.44  -28.45  -22.25     -28.45  -78.04  -64.38 -111.96
126   -32.20   -0.21  -24.19  -27.51  -22.53     -27.51  -73.58  -65.52  -86.72
127            -0.21  -24.96  -26.62  -22.70     -26.62  -70.69  -66.89  -80.96
128  -324.26   -0.22  -25.77  -25.77  -22.76     -25.77  -68.56  -68.56  -77.55
in band to aliased ratio =   36.65737
in band to distant aliased ratio =   61.01904
overlapped passband max deviation from flat =    0.40532
in band to weighted aliased ratio =   53.68086
*/
static int LdspkrAnalysisWindow[ 48] = {
    75,   -70,  -302,  -616,  -992, -1396, -1775, -2063, -2210, -2010,
 -1438,  -424,  1079,  3088,  5580,  8495, 11740, 15117, 18493, 21685,
 24510, 26798, 28407, 29233, 29233, 28407, 26798, 24510, 21685, 18493,
 15117, 11740,  8495,  5580,  3088,  1079,  -424, -1438, -2010, -2210,
 -2063, -1775, -1396,  -992,  -616,  -302,   -70,    75};

