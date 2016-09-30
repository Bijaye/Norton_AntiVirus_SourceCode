// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#define UPGRADE_KEY					"RemoteUpgrade"
#define TEMP_ERROR_KEY				UPGRADE_KEY"\\TmpErrors"

#define STATUS_MASK					0xff000000
#define PERCENT_MASK				0x00ff0000
#define ACTION_MASK					0x0000f000
#define ERROR_MASK					0x00000fff

#define PERCENT_SHIFT				16

#define _STATUS					"_Status"
#define _ERROR					"_Error"
#define _COMMAND				"_Command"

#define STS_INITIAL_MESSAGE			0x01000000
#define STS_STARTED_MESSAGE			0x02000000
#define STS_FINISHED_MESSAGE		0x03000000
#define STS_WILLNOT_UPGRADE			0x05000000
#define STS_REBOOT_NECESSARY		0x07000000
#define STS_USER_STOPPED			0x08000000
#define STS_COPYING_FILES			0x09000000
#define STS_DELETING_FILES			0x0a000000
#define STS_UPDATING_REGISTRY		0x0b000000
#define STS_CLEANUP_REGISTRY		0x0c000000
#define STS_ADDING_SHARES			0x0d000000
#define STS_REMOVING_SHARES			0x0e000000
#define STS_UPDATING_AMS2			0x0f000000
#define STS_STARTING_SERVICE		0x10000000
#define STS_STOPPING_SERVICE		0x11000000

#define ERR_READING_FILE			0x00005050
#define ERR_UPDATING_AMS2			0x00005060

#define INITIAL_STATE				0x00000000
#define STARTED_STATE				0x00000001
#define COMPLETE_STATE				0x00000002
#define ERROR_STATE					0x00000003
#define WAIT_STATE					0x00000004
#define TRAPPED_STATE				0x00010000
#define RELEASED_STATE				0x00020000

#define CMD_PAUSE					0x0001
#define CMD_RESUME					0x0002
#define CMD_CANCEL					0x0003
#define CMD_CLOSE					0x0004
#define CMD_ERRORS					0x0005

