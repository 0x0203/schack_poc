#ifndef SCHACK_PACKET_H
#define SCHACK_PACKET_H

enum ptype
{
    TEXT,
    VOICE,
    DATA,
    NAVIGATION,
    SCAN,
    PING,
    ACK,
    TELEMETRY,
    CANDC
};


const char *aegs_packets[] =
{
    "\x01\x02"
    "AEGS"
    "RADIO"
    "sender=Alice_Wonder\n"
    "receiver=Anton'sBackTooth\n"
    "type=TEXT\n"
    "data=is anyone out there? I'm out of gas!\n"
    "\x03\x04",

    "\x01\x02"
    "AEGS"
    "RADIO"
    "sender=Anton'sBackTooth\n"
    "receiver=Alice_Wonder\n"
    "type=TEXT\n"
    "data=You realize you're only asking those who are already inside your ship, right?\n"
    "\x03\x04",

    "\x01\x02"
    "AEGS"
    "RADIO"
    "sender=Alice_Wonder\n"
    "receiver=Anton'sBackTooth\n"
    "type=TEXT\n"
    "data=No, I'm pretty sure this is a broadcast...\n"
    "\x03\x04",

    "\x01\x02"
    "AEGS"
    "RADIO"
    "sender=Anton'sBackTooth\n"
    "receiver=Alice_Wonder\n"
    "type=TEXT\n"
    "data=Dude, you need to flip your coms switch. If it was set to broadcast you wouldn't be talking to me\n"
    "\x03\x04",

    "\x01\x02"
    "AEGS"
    "RADIO"
    "sender=Alice_Wonder\n"
    "receiver=Anton'sBackTooth\n"
    "type=TEXT\n"
    "data=STFU. I know what I'm doing\n"
    "\x03\x04",

    "\x01\x02"
    "AEGS"
    "RADIO"
    "sender=Anton'sBackTooth\n"
    "receiver=Alice_Wonder\n"
    "type=TEXT\n"
    "data=ok. whatever\n"
    "\x03\x04",

    "\x01\x02"
    "AEGS"
    "RADIO"
    "sender=Alice_Wonder\n"
    "receiver=Anton'sBackTooth\n"
    "type=TEXT\n"
    "data=Anybody got gas?\n"
    "\x03\x04",

    "\x01\x02"
    "AEGS"
    "RADIO"
    "sender=Alice_Wonder\n"
    "receiver=Anton'sBackTooth\n"
    "type=TEXT\n"
    "data=Hey, my ship won't move. What did you do?\n"
    "\x03\x04",
};

const char *anvl_packets[] =
{
    "\x01\x02"
    "ANVL"
    "RADIO"
    "sender=BobbyG\n"
    "receiver=Drone001\n"
    "type=CANDC\n"
    "data=<trans x=40 unit=m\\><rot pitch=0.04 unit=rad\\>\n"
    "\x03\x04",

    "\x01\x02"
    "ANVL"
    "RADIO"
    "sender=Drone001\n"
    "receiver=BobbyG\n"
    "type=ACK\n"
    "data=\x06\n"
    "\x03\x04",

    "\x01\x02"
    "ANVL"
    "RADIO"
    "sender=BobbyG\n"
    "receiver=Drone001\n"
    "type=CANDC\n"
    "data=<trans x=1 y=2 z=0.5 unit=km\\><rot roll=180 yaw=270 unit=deg\\>\n"
    "\x03\x04",

    "\x01\x02"
    "ANVL"
    "RADIO"
    "sender=Drone001\n"
    "receiver=BobbyG\n"
    "type=ACK\n"
    "data=\x06\n"
    "\x03\x04",

    "\x01\x02"
    "ANVL"
    "RADIO"
    "sender=BobbyG\n"
    "receiver=Drone001\n"
    "type=CANDC\n"
    "data=<scan target=derelict042\\><deploy tool=jaws\\>\n"
    "\x03\x04",

    "\x01\x02"
    "ANVL"
    "RADIO"
    "sender=Drone001\n"
    "receiver=BobbyG\n"
    "type=ACK\n"
    "data=\x06\n"
    "\x03\x04",

    "\x01\x02"
    "ANVL"
    "RADIO"
    "sender=BobbyG\n"
    "receiver=Drone001\n"
    "type=CANDC\n"
    "data=<extract target=all\\>\n"
    "\x03\x04",

    "\x01\x02"
    "ANVL"
    "RADIO"
    "sender=Drone001\n"
    "receiver=BobbyG\n"
    "type=ACK\n"
    "data=\x06\n"
    "\x03\x04",
};

const char *aopo_packets[] =
{
    "\x01\x02"
    "AOPO"
    "RADIO"
    "sender=Charlie'sA's\n"
    "receiver=NONE\n"
    "type=NAVIGATION\n"
    "data=<pos x=94 y=50 z=11\\><dist unit=km>5898030303<\\dist>\n"
    "\x03\x04",

    "\x01\x02"
    "AOPO"
    "RADIO"
    "sender=Charlie'sA's\n"
    "receiver=NONE\n"
    "type=NAVIGATION\n"
    "data=<gps><request><relay_update type=pos\\><\\request><\\gps>\n"
    "\x03\x04",

    "\x01\x02"
    "AOPO"
    "RADIO"
    "sender=Charlie'sA's\n"
    "receiver=NONE\n"
    "type=NAVIGATION\n"
    "data=<ais command=send><pos\\><\\ais>\n"
    "\x03\x04",

    "\x01\x02"
    "AOPO"
    "RADIO"
    "sender=Charlie'sA's\n"
    "receiver=NONE\n"
    "type=NAVIGATION\n"
    "data=<starmap><command type=chart_new_point><pos theta=260 gamma=122 rad=4022 unit=km\\><\\command><\\starmap>\n"
    "\x03\x04",
};

const char *banu_packets[] =
{
    "\x01\x02"
    "BANU"
    "RADIO"
    "sender=DanielleSteals\n"
    "receiver=NONE\n"
    "type=PING\n"
    "data=ICMP\n"
    "\x03\x04",
    "\x01\x02"
    "BANU"
    "RADIO"
    "sender=DanielleSteals\n"
    "receiver=NONE\n"
    "type=PING\n"
    "data=ICMP\n"
    "\x03\x04",
    "\x01\x02"
    "BANU"
    "RADIO"
    "sender=DanielleSteals\n"
    "receiver=NONE\n"
    "type=PING\n"
    "data=ICMP\n"
    "\x03\x04",
    "\x01\x02"
    "BANU"
    "RADIO"
    "sender=DanielleSteals\n"
    "receiver=NONE\n"
    "type=PING\n"
    "data=ICMP\n"
    "\x03\x04",
    "\x01\x02"
    "BANU"
    "RADIO"
    "sender=DanielleSteals\n"
    "receiver=NONE\n"
    "type=PING\n"
    "data=ICMP\n"
    "\x03\x04",
};

const char *cnou_packets[] =
{
    "\x01\x02"
    "CNOU"
    "RADIO"
    "sender=EchoMicroDot\n"
    "receiver=BROADCAST\n"
    "type=SCAN\n"
    "data=nmap -sl WSAN://*\n"
    "\x03\x04",

    "\x01\x02"
    "CNOU"
    "RADIO"
    "sender=EchoMicroDot\n"
    "receiver=BROADCAST\n"
    "type=SCAN\n"
    "data=nmap -sn WSAN://*\n"
    "\x03\x04",

    "\x01\x02"
    "CNOU"
    "RADIO"
    "sender=EchoMicroDot\n"
    "receiver=BROADCAST\n"
    "type=SCAN\n"
    "data=nmap -sN WSAN://*\n"
    "\x03\x04",

    "\x01\x02"
    "CNOU"
    "RADIO"
    "sender=EchoMicroDot\n"
    "receiver=BROADCAST\n"
    "type=SCAN\n"
    "data=nmap -sV WSAN://*\n"
    "\x03\x04",
};

const char *crus_packets[] =
{
    "\x01\x02"
    "CRUS"
    "RADIO"
    "sender=TurkeyFranks\n"
    "receiver=SHIP_COMS\n"
    "type=VOICE\n"
    "data=This is your captain speaking. The seatbelt sign has now been turned on. Please return to your seats and refraing from looking out the port-side windows.\n"
    "\x03\x04",

    "\x01\x02"
    "CRUS"
    "RADIO"
    "sender=TurkeyFranks\n"
    "receiver=SHIP_COMS\n"
    "type=VOICE\n"
    "data=Tower, this is TurkeyFranks, declaring an emergency. Requesting priority landing on pad 06 right.\n"
    "\x03\x04",

    "\x01\x02"
    "CRUS"
    "RADIO"
    "sender=TurkeyFranks\n"
    "receiver=SHIP_COMS\n"
    "type=VOICE\n"
    "data=Uhhh... Sorry about that folks, please disregard last. Jimmy had the radio switched to the wrong band... \n"
    "\x03\x04",
};

const char *drak_packets[] =
{
    "\x01\x02"
    "DRAK"
    "RADIO"
    "sender=Grettle_ate_Hansel\n"
    "receiver=CARGO_HOLD\n"
    "type=TEXT\n"
    "data=Fuzz incomming. ETA 2 min. Secure package.\n"
    "\x03\x04",
    "\x01\x02"
    "DRAK"
    "RADIO"
    "sender=CARGO_HOLD\n"
    "receiver=Grettle_ate_Hansel\n"
    "type=TEXT\n"
    "data=HOW THE FRIG AM I SUPPOSED TO HIDE A BLOODY X1??\n"
    "\x03\x04",
    "\x01\x02"
    "DRAK"
    "RADIO"
    "sender=Grettle_ate_Hansel\n"
    "receiver=CARGO_HOLD\n"
    "type=TEXT\n"
    "data=Figure it out, man, or I'm going to shoot you and claim you were holding me hostage.\n"
    "\x03\x04",
    "\x01\x02"
    "DRAK"
    "RADIO"
    "sender=CARGO_HOLD\n"
    "receiver=Grettle_ate_Hansel\n"
    "type=TEXT\n"
    "data=FU\n"
    "\x03\x04",
    "\x01\x02"
    "DRAK"
    "RADIO"
    "sender=Grettle_ate_Hansel\n"
    "receiver=CARGO_HOLD\n"
    "type=TEXT\n"
    "data=WTF!?!?!? WHY DID YOU JUST DEEP 6 THAT S***!? THAT WAS MY PAYDAY! NOW I REALY AM GOING TO K\n"
    "\x03\x04",
};

const char *espr_packets[] =
{
    "\x01\x02"
    "ESPR"
    "RADIO"
    "sender=HansGruberDidNothingWrong\n"
    "receiver=Drone432\n"
    "type=CANDC\n"
    "data=<trans x=4099 y=34 unit=m\\><rot yaw=180 unit=deg\\>\n"
    "\x03\x04",
    "\x01\x02"
    "ESPR"
    "RADIO"
    "sender=HansGruberDidNothingWrong\n"
    "receiver=Drone432\n"
    "type=CANDC\n"
    "data=<trans x=4099 y=34 unit=m\\><rot yaw=180 unit=deg\\>\n"
    "\x03\x04",
    "\x01\x02"
    "ESPR"
    "RADIO"
    "sender=HansGruberDidNothingWrong\n"
    "receiver=Drone432\n"
    "type=CANDC\n"
    "data=<trans x=4099 y=34 unit=m\\><rot yaw=180 unit=deg\\>\n"
    "\x03\x04",
    "\x01\x02"
    "ESPR"
    "RADIO"
    "sender=HansGruberDidNothingWrong\n"
    "receiver=Drone432\n"
    "type=CANDC\n"
    "data=<trans x=4099 y=34 unit=m\\><rot yaw=180 unit=deg\\>\n"
    "\x03\x04",
    "\x01\x02"
    "ESPR"
    "RADIO"
    "sender=HansGruberDidNothingWrong\n"
    "receiver=Drone432\n"
    "type=CANDC\n"
    "data=<trans x=4099 y=34 unit=m\\><rot yaw=180 unit=deg\\>\n"
    "\x03\x04",
    "\x01\x02"
    "ESPR"
    "RADIO"
    "sender=Drone432\n"
    "receiver=HasGruberDidNothingWrong\n"
    "type=ACK\n"
    "data=\x06\n"
    "\x03\x04",
    "\x01\x02"
    "ESPR"
    "RADIO"
    "sender=HansGruberDidNothingWrong\n"
    "receiver=HansGrubersPriest\n"
    "type=TEXT\n"
    "data=Jeeeessss. Finally!\n"
    "\x03\x04",
};

const char *krig_packets[] =
{
    "\x01\x02"
    "KRIG"
    "RADIO"
    "sender=UglyIngrid\n"
    "receiver=NONE\n"
    "type=NAVIGATION\n"
    "data=<gps command=update\\>\n"
    "\x03\x04",
    "\x01\x02"
    "KRIG"
    "RADIO"
    "sender=UglyIngrid\n"
    "receiver=NONE\n"
    "type=NAVIGATION\n"
    "data=<gps command=update\\>\n"
    "\x03\x04",
    "\x01\x02"
    "KRIG"
    "RADIO"
    "sender=UglyIngrid\n"
    "receiver=NONE\n"
    "type=NAVIGATION\n"
    "data=<gps command=update\\>\n"
    "\x03\x04",
};

const char *misc_packets[] =
{
    "\x01\x02"
    "MISC"
    "RADIO"
    "sender=Jack-b-trippn\n"
    "receiver=any\n"
    "type=DATA\n"
    "data=\x84\xc9\xc9\xc9laaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n"
    "\x03\x04",

    "\x01\x02"
    "MISC"
    "RADIO"
    "sender=Jack-b-trippn\n"
    "receiver=any\n"
    "type=DATA\n"
    "data=#!/bin/sh\n"
    "\x03\x04",

    "\x01\x02"
    "MISC"
    "RADIO"
    "sender=Jack-b-trippn\n"
    "receiver=any\n"
    "type=DATA\n"
    "data=\x7f\x45\x4c\x46\x02\x01\x01\x84\xc9\xc9\xc9\xc9\x5f\xc9\xc9\x5f\xc9\xc9\x5f\xc9\xc9\x5f\xc9\xc9\x5f\xc9\xc9\x5f\n"
    "\x03\x04",
};

const char *rsi_packets[] =
{
    "\x01\x02"
    "RSI_"
    "RADIO"
    "sender=HKeller'sShootingSchool\n"
    "receiver=anyone\n"
    "type=TEXT\n"
    "data=urrr... help?\n"
    "\x03\x04",
    "\x01\x02"
    "RSI_"
    "RADIO"
    "sender=HKeller'sShootingSchool\n"
    "receiver=anyone\n"
    "type=TEXT\n"
    "data=I think I'm on fire\n"
    "\x03\x04",
    "\x01\x02"
    "RSI_"
    "RADIO"
    "sender=HKeller'sShootingSchool\n"
    "receiver=anyone\n"
    "type=TEXT\n"
    "data=does the rsi suit have a fire extinguisher built into the helmet?\n"
    "\x03\x04",
};

const char *mobi_packets[] =
{
    "\x01\x02"
    "MOBI"
    "RADIO"
    "sender=Llama's_New_Groove\n"
    "receiver=self\n"
    "type=SCAN\n"
    "data=<scan type=intruder\\>\n"
    "\x03\x04",
    "\x01\x02"
    "MOBI"
    "RADIO"
    "sender=Llama's_New_Groove\n"
    "receiver=self\n"
    "type=CANDC\n"
    "data=<equipment_manager component=torso><equip><item type=helmet id=94574\\><\\equip><\\equipment_manager>\n"
    "\x03\x04",
    "\x01\x02"
    "MOBI"
    "RADIO"
    "sender=Llama's_New_Groove\n"
    "receiver=ship\n"
    "type=TEXT\n"
    "data=Mission update incoming\n"
    "\x03\x04",
    "\x01\x02"
    "MOBI"
    "RADIO"
    "sender=Llama's_New_Groove\n"
    "receiver=self\n"
    "type=DATA\n"
    "data=<mission id=48762><accept\\><\\mission>\n"
    "\x03\x04",
};

const struct
{
    const char **packets;
    size_t	count;
} data_packets[] = {
    {aegs_packets,NK_LEN(aegs_packets)},
    {anvl_packets,NK_LEN(anvl_packets)},
    {aopo_packets,NK_LEN(aopo_packets)},
    {banu_packets,NK_LEN(banu_packets)},
    {cnou_packets,NK_LEN(cnou_packets)},
    {crus_packets,NK_LEN(crus_packets)},
    {drak_packets,NK_LEN(drak_packets)},
    {espr_packets,NK_LEN(espr_packets)},
    {krig_packets,NK_LEN(krig_packets)},
    {misc_packets,NK_LEN(misc_packets)},
    {rsi_packets, NK_LEN(rsi_packets)},
    {mobi_packets,NK_LEN(mobi_packets)},
    };

#if 0
const char **data_packets[] =
{
    aegs_packets,
    anvl_packets,
    aopo_packets,
    banu_packets,
    cnou_packets,
    crus_packets,
    drak_packets,
    espr_packets,
    krig_packets,
    misc_packets,
    rsi_packets,
    mobi_packets
};
#endif

#endif

