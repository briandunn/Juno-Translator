//
// Juno2cc
//
// 2006 Brian Dunn <job17and9@yahoo.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

//
// Based on juno_rose by Joost Vervante Damad,
// which was based on midiroute.c by Matthias Nagorni
//
#include <iostream>
using std::cerr;
using std::endl;

#include <alsa/asoundlib.h>

#define MY_NAME "Juno2cc"

static bool open_seq(snd_seq_t **seq_handle, int& in_port, int& out_port) {

	if( snd_seq_open(seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0 ) < 0 ) {
		cerr << "Error opening ALSA sequencer." << endl;
		return false;
	}
	snd_seq_set_client_name(*seq_handle, MY_NAME);
	if( ( in_port = snd_seq_create_simple_port( *seq_handle, MY_NAME "IN", SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE, SND_SEQ_PORT_TYPE_APPLICATION ) ) < 0 ) {
		cerr << "Error creating sequencer port." << endl;
		return false;
	}
	if( ( out_port = snd_seq_create_simple_port( *seq_handle, MY_NAME "OUT", SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ, SND_SEQ_PORT_TYPE_APPLICATION ) ) < 0 ) {
		cerr << "Error creating sequencer port." << endl;
		return false;
	}
	return true;
}

/*
CC	CC(dec)	Description		Parameter Number
10	16		LFO Rate		00
11	17		LFO Delay Time	01
12	18		DCO_LFO			02
13	19		DCO_PWM			03
14	20		NOISE			04
15	21		VCF_cutoff		05
16	22		VCF_resonance	06
17	23		VCF_ENV			07
18	24		VCF_LFO			08
19	25		VCF_KYBD		09
1A	26		VCA				0A
1B	27		ENV_A			0B
1C	28		ENV_D			0C
1D	29		ENV_S			0D
1E	30		ENV_R			0E
1F	31		SUB				0F
*/

void convert( snd_seq_t *seq_handle, int out_port ) {
	snd_seq_event_t *ev;
	//char data[7];
	
	do {
		snd_seq_event_input( seq_handle, &ev );
		snd_seq_ev_set_subs( ev );
		snd_seq_ev_set_direct( ev );
		if( ev->type == SND_SEQ_EVENT_SYSEX ) {
			unsigned char * knobVal_ptr;
			unsigned char chan;
			unsigned int cc;
			int val;
			/*
			char data[7];								//a map of the juno's messages
			data[0] = 0xF0;								// Begin sysEx
			data[1] = 0x41;								// Roland Vendor ID
			data[2] = 0x32;								// Function Type
			data[3] = ev->data.control.channel;			// Midi Channel
			data[4] = ev->data.control.param - 0x10;	// Parameter Number
			data[5] = ev->data.control.value;			// Value
			data[6] = 0xF7;								// End sysEx
			*/
			knobVal_ptr = ( unsigned char * ) ev->data.ext.ptr;
			chan = (unsigned char) *(knobVal_ptr + 3);
			cc = (unsigned int) *(knobVal_ptr + 4) + 0x10;
			val = (int) *(knobVal_ptr + 5);
			snd_seq_ev_set_controller( ev, chan, cc, val );
		}
		snd_seq_ev_set_source(ev, out_port);
	snd_seq_event_output_direct(seq_handle, ev);
	snd_seq_free_event(ev);
	}while( snd_seq_event_input_pending( seq_handle, 0 ) > 0 );
}

int main(int argc, char *argv[]) {
	snd_seq_t *seq_handle;
	int in_port, out_port;
	int npfd;
	struct pollfd *pfd;

	if( ! open_seq( &seq_handle, in_port, out_port ) ) {
		cerr << "ALSA Error." << endl;
		exit(1);
	}
	npfd = snd_seq_poll_descriptors_count( seq_handle, POLLIN );
	pfd = ( struct pollfd *) alloca( npfd * sizeof( struct pollfd ) );
	snd_seq_poll_descriptors( seq_handle, pfd, npfd, POLLIN );
	while (true) {
		if( poll( pfd, npfd+1, 1000 ) > 0 ) {
			convert( seq_handle, out_port );
		}
	}
}
