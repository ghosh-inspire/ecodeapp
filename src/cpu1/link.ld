SECTIONS {
	.cpu1_data 0x20000000 : {
		KEEP(*( .cpu1_data ))
	}
}

INSERT AFTER .data;
