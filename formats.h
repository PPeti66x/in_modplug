typedef struct _FormatsListItem
{
	char * ext;
	char * name;
} FormatsListItem;

// A good source for sample files to check what formats are really supported:
// "ftp://ftp.modland.com/pub/modules/"
// An another one:
// "https://amp.dascene.net/faq.php"

FormatsListItem Formats [ ] = {
	//Do not use multiple file extensions for a single list item.
	//Extensions are in capped letters and ends by semicolon.
	//NOTE: No compressed module support, no MOL playlist file support.
	//items  1-10
	//----- most standard formats -----
	{"MOD;\0", "MOD	ProTracker\0"},
	{"STM;\0", "STM	ScreamTracker 2\0"},
	// STX (v1.0, v1.1) format not supported (transition format btw. STM and S3M formats)
	{"S3M;\0", "S3M	ScreamTracker 3\0"},
	{"FST;\0", "FST	FastTracker I\0"},
	{"XM;\0", "XM	FastTracker II\0"},
	{"IT;\0", "IT	Impulse Tracker\0"},
	//----- other regular formats -----
	{"669;\0", "669	669 Composer, UNIS 669\0"},
	{"AMF;\0", "AMF	DSMI AMF / Asylum\0"},
	//AMS - not all files are recognized
	{"AMS;\0", "AMS	Extreme Tracker / Velvet Studio\0"},
	{"DBM;\0", "DBM	DigiBooster Pro\0"},
	//items 11-20
	{"DMF;\0", "DMF	X-Tracker\0"},
	{"DSM;\0", "DSM	DSIK Format\0"},
	{"FAR;\0", "FAR	Farandole Composer\0"},
	{"M15;\0", "M15	Ultimate SoundTracker\0"},
	{"MDL;\0", "MDL	DigiTrakker\0"},
	{"MED;\0", "MED	OctaMed\0"},
	{"MT2;\0", "MT2	MadTracker 2\0"},
	{"MTM;\0", "MTM	MultiTracker\0"},
	{"NST;\0", "NST	NoiseTracker\0"},
	{"OKT;\0", "OKT	Oktalyzer\0"},
	//items 21-30
	{"PTM;\0", "PTM	PolyTracker\0"},
	{"STK;\0", "STK	Ultimate Soundtracker\0"},
	{"ULT;\0", "ULT	UltraTracker\0"},
	{"WOW;\0", "WOW	Grave Composer\0"},
	//STP files are not supported.
	//{"STP;\0", "STP	Soundtracker Pro II\0"},
	//ST26 files with MOD extension are not supported (are ST26 files different?)
	//{"ST26;\0", "ST26	SoundTracker 2.6 / Ice Tracker\0"},
	//J2B is not supported (the load_j2b.cpp is empty)
	//{"J2B;\0", "J2B	Jazz Jackrabbit 2 Music\0"},
	//----- container formats and compressed modules -----
	//Only the "PSM New Format" is supported, the old "PSM 16" is not.
	{"PSM;\0", "PSM	Epic Megagames MASI / ProTracker Studio\0"},
	{"UMX;\0", "UMX	Unreal Music\0"},
	{"MO3;\0", "MO3	MO3 mod format (via unmo3.dll)\0"},
	//----- MIDI formats -----
	{"MID;\0", "MID	MIDI file\0"},
	//RIFF-MIDI files (*.RMI, *.RMID) are not supported? Or it is only buggy?
	{"RMI;\0", "RMI	RIFF-MIDI file\0"},
	{"SMI;\0", "SMI	Standard MIDI File\0"},
	//items 31-31
	//ABC notation reader is very sensiteve for unwanted data, eg. empty line at file start.
	{"ABC;\0", "ABC	ABC notation (MIDI)\0"}
};
