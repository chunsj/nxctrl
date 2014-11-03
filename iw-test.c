#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <err.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/wireless.h>

#define NOISE_DBM_SANE_MIN -127

struct iw_levelstat {
  float signal;
  float noise;
  uint8_t flags;
};

struct iw_stat {
  struct iw_range range;
  struct iw_statistics stat;
  struct iw_levelstat dbm;
};

static void
iw_getstat_real (struct iw_statistics *stat) {
  struct iwreq wrq;
  int skfd = socket(AF_INET, SOCK_DGRAM, 0);

  if (skfd < 0) {
    fprintf(stderr, "%s: cannot open socket\n", "iw_getstat_real");
    return;
  }

  wrq.u.data.pointer = (caddr_t)stat;
  wrq.u.data.length = sizeof(*stat);
  wrq.u.data.flags = 0;
  strncpy(wrq.ifr_name, "wlan0", IFNAMSIZ);

  if (ioctl(skfd, SIOCGIWSTATS, &wrq) < 0) {
    if (errno != EOPNOTSUPP) {
      fprintf(stderr, "cannot obtain iw statistics\n");
      return;
    }
    errno = 0;
    memset(&wrq, 0, sizeof(wrq));
  }
  close(skfd);
}

static inline int
u8_to_dbm (const int power) {
  return power > 63 ? power - 0x100 : power;
}

static inline double
mw2dbm (const double in) {
  return 10.0 * log10(in);
}

static void
iw_sanitize (struct iw_range *range, struct iw_quality *qual,
             struct iw_levelstat *dbm) {
  memset(dbm, 0, sizeof(*dbm));

  if (qual->level != 0 || (qual->updated & (IW_QUAL_DBM | IW_QUAL_RCPI))) {
    if (qual->updated & IW_QUAL_RCPI) {
      if (!(qual->updated & IW_QUAL_LEVEL_INVALID))
        dbm->signal = (double)(qual->level / 2.0) - 110.0;
      if (!(qual->updated & IW_QUAL_NOISE_INVALID))
        dbm->noise = (double)(qual->noise / 2.0) - 110.0;
    } else if ((qual->updated & IW_QUAL_DBM) || (qual->level > range->max_qual.level)) {
      if (!(qual->updated & IW_QUAL_LEVEL_INVALID))
        dbm->signal = u8_to_dbm(qual->level);
      if (!(qual->updated & IW_QUAL_NOISE_INVALID))
        dbm->noise = u8_to_dbm(qual->noise);
    } else {
      if (!(qual->updated & IW_QUAL_LEVEL_INVALID))
        dbm->signal = mw2dbm(qual->level);
      if (!(qual->updated & IW_QUAL_NOISE_INVALID))
        dbm->noise = mw2dbm(qual->noise);
    }
  } else
    qual->updated |= IW_QUAL_ALL_INVALID;

  if (qual->updated & IW_QUAL_LEVEL_INVALID)
    qual->updated |= IW_QUAL_NOISE_INVALID;
  if (dbm->noise <= NOISE_DBM_SANE_MIN)
    qual->updated |= IW_QUAL_NOISE_INVALID;
}

static void
iw_getstat (struct iw_stat *iw) {
  memset(&iw->stat, 0, sizeof(iw->stat));
  iw_getstat_real(&iw->stat);
  iw_sanitize(&iw->range, &iw->stat.qual, &iw->dbm);
}

static inline double
ewma (double mavg, double sample, double weight) {
  return mavg == 0 ? sample : weight * mavg + (1.0 - weight) * sample;
}

int
main (void) {
  struct iw_stat iws;
  static float qual = 0.00;
  iw_getstat(&iws);
  qual = ewma(qual, iws.stat.qual.qual, 0 /* meter decay */ / 100.0);
  printf("%.2f\n", qual);
  printf("%d\n", iws.range.max_qual.qual);
  printf("%0.f%% %.2f %.2f\n", (1e2 * qual)/iws.range.max_qual.qual, iws.dbm.signal, iws.dbm.noise);
  return 0;
}
