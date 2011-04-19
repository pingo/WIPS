#ifndef __PROJECT_CONF_H
#define __PROJECT_CONF_H

#undef  NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE
#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 16

#undef  NETSTACK_CONF_RDC
#undef  NETSTACK_CONF_MAC
#define NETSTACK_CONF_RDC contikimac_driver
#define NETSTACK_CONF_MAC csma_driver

/* #undef  NETSTACK_CONF_MAC
   #define NETSTACK_CONF_MAC xmac_driver */

/* #undef  NETSTACK_CONF_FRAMER
   #define NETSTACK_CONF_FRAMER framer_802154 */

/* #undef  DEBUG
   #define DEBUG 1 */

#endif /* __PROJECT_CONF_H */

