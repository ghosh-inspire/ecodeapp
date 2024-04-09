SECTIONS {
	.cpu2_data 0x80010 : {
		KEEP(*( .cpu2_data ))
	}
}

INSERT AFTER .data;
