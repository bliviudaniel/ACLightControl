#ifndef _RADIONET_CFG_H_
#define _RADIONET_CFG_H_

// Network nodes names
const char nodesNamesRo[][11] = 
{
   " PORTABIL ",
   "DORMITOR 1",
   "DORMITOR B",
   "DORMITOR F",
   "DORMITOR 0",
   "SUFRAGERIE",
   "  AFARA   ",
   "  TERASA  ",
   "  SOLAR   ",
   " MAGAZIE  ",
   "  PASARI  ",
   "   BECI   ",
   "   POD    ",
   "  BIROU   "
};

const char nodesNamesEn[][11] = 
{
   " PORTABLE ",
   "BEDROOM 1 ",
   "BEDROOM B ",
   "BEDROOM G ",
   "BEDROOM 0 ",
   "LIVINGROOM",
   " OUTDOOR  ",
   " TERRACE  ",
   "GREENHOUSE",
   " STORAGE  ",
   "BIRDSROOM ",
   " BASEMENT ",
   "  ATTIC   ",
   "  OFFICE  "
};

// Radio network node IDs
typedef enum te_nodes
{
   RADIO_NODE0_ID = 0,
   RADIO_NODE1_ID = 1,
   RADIO_NODE2_ID = 2,
   RADIO_NODE3_ID = 3,
   RADIO_NODE4_ID = 4,
   RADIO_NODE5_ID = 5,
   RADIO_NODE6_ID = 6,
   RADIO_NODE7_ID = 7,
   RADIO_NODE8_ID = 8,
   RADIO_NODE9_ID = 9,
   RADIO_NODE10_ID = 10,
   RADIO_NODE11_ID = 11,
   RADIO_NODE12_ID = 12,
   RADIO_NODE13_ID = 13,
   RADIO_NR_NODES
}TE_Nodes;

// Radio(ESP-NOW/WiFi) node packet
typedef struct ts_radio_node_packet
{
   uint8_t  head;
   uint8_t  nodeID;
   int8_t   tempInt;
   uint8_t  tempDec;
   uint8_t  humiInt;
   uint8_t  humiDec;
   uint8_t  presH;
   uint8_t  presL;
   uint8_t  tvocH;
   uint8_t  tvocL;
   uint8_t  eco2H;
   uint8_t  eco2L;
   uint8_t  battInt;
   uint8_t  battDec;
   uint8_t  tail;
   uint8_t  checksum;
}TS_RadioNodePacket;

#endif //_RADIONET_CFG_H_
