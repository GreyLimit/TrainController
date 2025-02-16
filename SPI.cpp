//
//	SPI.h
//	=====
//
//	Provide a generic interface to the SPI driver hardware
//	proivding the "controller" functionality.  There is,
//	currently, no "peripheral" functionality.
//


#include "SPI.h"
#include "Code_Assurance.h"
#include "Errors.h"
#include "Stats.h"

#include "Trace.h"
#ifdef DEBUGGING_ENABLED
#include "Console.h"
#endif

//
//	The table itself.  These figures reflect the the 8-bit AVR
//	system hardware.
//
//	Note/	The field rate is only used as a consistency check.
//		Also note that the entries must be increasing order
//		of rate divisor (ie faster clocks are tested before
//		slower clocks).
//
static const SPI_Device::clock_divisor SPI_Device::clock_table[] PROGMEM = {
	//
	//	Rate	Shift,	SPI2X	SPR1	SPR0
	//
	{	2,	1,	true,	false,	false	},	// F(osc)/2
	{	4,	2,	false,	false,	false	},	// F(osc)/4
	{	8,	3,	true,	false,	true	},	// F(osc)/8
	{	16,	4,	false,	false,	true	},	// F(osc)/16
	{	32,	5,	true,	true,	false	},	// F(osc)/32
	{	64,	6,	false,	true,	false	},	// F(osc)/64
	{	128,	7,	false,	true,	true	},	// F(osc)/128
	//
	//	Mark end of table (rate and shift zero).
	//
	{	0,	0,	false,	false,	false	}
};

//
//	Select the clock divisor to be used (with respect to the
//	base clock frequency F(osc).
//
//	Return true on success, false on invalid divisor.
//
bool SPI_Device::set_clock( SPI_Registers *device, word clock ) {

	STACK_TRACE( "bool SPI_Device::set_clock( SPI_Registers *device, word clock )" );
	
	const clock_divisor	*c;
	byte			s;
		
	c = clock_table;
	while(( s = progmem_read_byte( c->shift ))) {

		//
		//	Just a little consistency checking.
		//
		ASSERT(( 1 << s ) == c->rate );

		//
		//	Check to see if the requested clock speed is
		//	greater than the divided clock speed.  If so
		//	then we have found the first configurable speed
		//	slower than the requested speed.
		//
		if(( cpu_hz >> s ) <= clock ) {
			//
			//	Found, so apply the changes.
			//
			device->spi2x((bool)( progmem_read_byte( c->spi2x )));
			device->spr1((bool)( progmem_read_byte( c->spr1 )));
			device->spr0((bool)( progmem_read_byte( c->spr0 )));
			return( true );
		}
		c++;
	}
	return( false );
}

//
//	Start transaction puts the transaction addressed by
//	_active into action.
//
void SPI_Device::start_trans( void ) {

	STACK_TRACE( "void SPI_Device::start_trans( void )" );

	//
	//	This must *never* be called outside an interrupt
	//	safe code section.  This is either an ISR() routine
	//	or a "Critical code" section.
	//
	ASSERT( Critical::critical_code());

	//
	//	Of course we anticipate that there is a transaction
	//	to be started and its target is valid.
	//
	ASSERT( _active != NIL( spi_trans ));
	ASSERT( _active->target < _targets );

	//
	//	Pointer to the new target.
	//
	spi_target *q = &( _target[ _active->target ]);

	//
	//	We now load the SPI hardware with our configuration
	//
	_dev->load( &( q->configuration ));

	//
	//	Now enable the target chip
	//
	q->pin->set( q->enable );

	//
	//	Kick off the transaction by sending a byte of data.
	//
	if( _active->send ) {
		//
		//	We have data to send, which is normal, so we can
		//	simply let the process roll using the normal
		//	exchange process.
		//
		spi_event();
	}
	else {
		//
		//	Here we have a special case where no data is being sent.
		//	We must initiate this by sending an initial packing byte.
		//
		_dev->write( packing_byte );
	}
	
	//
	//	We're off!
	//
}

//
//	Stop transaction end the transaction addressed by
//	_active and initiates a following if available.
//
void SPI_Device::stop_trans( void ) {

	STACK_TRACE( "void SPI_Device::stop_trans( void )" );

	//
	//	This must *never* be called outside an interrupt
	//	safe code section.
	//
	ASSERT( Critical::critical_code());

	//
	//	Of course we anticipate that there is a transaction
	//	to be stopped.
	//
	ASSERT( _active != NIL( spi_trans ));
	ASSERT( _active->target < _targets );

	//
	//	Pointer to the target configuration.
	//
	spi_target *q = &( _target[ _active->target ]);

	//
	//	For juggling the queues.
	//
	spi_trans	*t;

	//
	//	Disable the target chip SPI interface
	//
	q->pin->set( !q->enable );

	//
	//	Shutdown the SPI interface.
	//
	_dev->clear();
	
	//
	//	Signal the flag that we're done.
	//
	_active->flag->release();

	//
	//	Remove from the active list and add to free list.
	//
	t = (spi_trans *)_active;
	if(( _active = t->next ) == NIL( spi_trans )) _tail = &_active;
	t->next = (spi_trans *)_free;
	_free = t;

	//
	//	If there is another transaction pending
	//	then we can fire that off now.
	//
	if( _active ) start_trans();
}

//
//	This is the constructor which is passed in the
//	address of the registers for the SPI device to
//	be managed and the interrupt redirection register
//	for the device.
//
SPI_Device::SPI_Device( SPI_Registers *device ) {

	ASSERT( device != NIL( SPI_Registers ));

	//
	//	Save our hardware address and clear targets table.
	//
	_dev = device;
	_targets = 0;
	
	//
	//	Organise the transactions queue.
	//
	_free = NIL( spi_trans );
	for( byte i = 0; i < spi_queue_size; i++ ) {
		_queue[ i ].next = (spi_trans *)_free;
		_free = &( _queue[ i ]);
	}
	_active = NIL( spi_trans );
	_tail = &_active;
	
	//
	//	We are ready to go.
	//
}

//
//	Convert an unsigned long clock speed (in Hz) to the
//	word value used by the new_target routine.
//
word SPI_Device::hz( unsigned long clock ) {

	STACK_TRACE( "word SPI_Device::hz( unsigned long clock )" );

	ASSERT( clock <= KHZ_TO_HZ( MAXIMUM_WORD ));
	ASSERT( clock >= KHZ_TO_HZ( 1 ));

	return( HZ_TO_KHZ( clock ));
}


//
//	Add a new target to the SPI manager.  This will return
//	true on success with an arbitrary target handle or
//	false if there was an issue with the configuration.
//
//	target		Returned target number for this SPI
//			attached device.
//
//	speed		The bus speed the device requires in
//			units of 1024 Hz.  The final speed
//			configured will be no faster than this,
//			but could be much slower.
//
//	lsb		True if the remote device is least
//			significant bit first, false if it is
//			most significant bit first.
//
//	cpol, cpha	Bus signal configuration.
//
//	pin		The pin used to enable the target device
//
//	enable		The "state" to set the pin to enable
//			the target device.
//
bool SPI_Device::new_target( byte *target, word speed, bool lsb, bool cpol, bool cpha, Pin_IO *pin, bool enable ) {

	STACK_TRACE( "bool SPI_Device::new_target( byte *target, word speed, bool cpol, bool cpha, Pin_IO *pin, bool enable )" );
	
	ASSERT( target != NIL( byte ));
	ASSERT( pin != NIL( Pin_IO ));

	spi_target	*t;

	//
	//	Check target table availability and assign an entry
	//	if space is available.
	//
	if( _targets >= spi_max_targets ) {
		errors.log_error( SPI_TARGET_TABLE_FULL, _targets );
		return( false );
	}
	//
	//	Save new target number and address of target record.
	//
	t = &( _target[( *target = _targets )]);

	//
	//	Set the clock speed/divisor
	//
	if( !set_clock( &( t->configuration ), speed )) {
		errors.log_error( SPI_INVALID_CLOCK_SPEED, speed );
		return( false );
	}

	//
	//	Set bit ordering.
	//
	t->configuration.dord( lsb );

	//
	//	Set up clock characteristics...
	//
	t->configuration.cpol( cpol );
	t->configuration.cpha( cpha );

	//
	//	Finally we pre-set the interrupt and SPI enable bits
	//	so that loading the configuration automatically starts
	//	the SPI interface.
	//
	t->configuration.spie( true );
	t->configuration.spe( true );

	//
	//	Save the chip enable details and perform initial
	//	chip disable.
	//
	t->pin = pin;
	t->enable = enable;
	pin->set( !enable );
	
	//
	//	Definitely done now, increase targets count before
	//	returning.
	//
	_targets++;
	return( true );
}

//
//	Exchange data with a registered target.  Returns true
//	if the request is queued, false otherwise.
//
bool SPI_Device::exchange( byte target, byte send, byte recv, byte *buffer, Signal *flag ) {

	STACK_TRACE( "bool SPI_Device::exchange( byte target, byte send, byte recv, byte *buffer, Signal *flag )" );

	spi_trans	*t;
	bool		start;

	ASSERT( flag != NIL( Signal ));
	ASSERT( buffer != NIL( byte ));
	ASSERT( target < _targets );
	ASSERT(( send + recv ) > 0 );

	//
	//	Get a free record that we can build the request into.
	//
	{
		Critical code;

		if(( t = (spi_trans *)_free ) == NIL( spi_trans )) {
			errors.log_error( SPI_QUEUE_FULL, target );
			return( false );
		}
		_free = t->next;
	}

	//
	//	Fill in the record...
	//
	t->target = target;
	t->send = send;
	t->recv = recv;
	t->sending = t->receiving = buffer;
	t->flag = flag;
	t->next = NIL( spi_trans );

	//
	//	Now append to the end of the queue.
	//
	{
		Critical code;

		//
		//	Remember if the queue was empty before we added
		//	this request.
		//
		start = ( _active == NIL( spi_trans ));

		//
		//	Link this into the queue.
		//
		*_tail = t;
		_tail = (volatile spi_trans **)&( t->next );

		//
		//	Now, if the queue *was* empty then we need to
		//	nudge the SPI system into performing it.
		//
		if( start ) start_trans();
	}

	//
	//	Done!
	//
	return( true );
}

//
//	Exchange a single byte with the attached device.
//
void SPI_Device::spi_event( void ) {

	STACK_TRACE( "void SPI_Device::spi_event( void )" );

	//
	//	This must *never* be called outside an interrupt
	//	safe code section.  This is either an ISR() routine
	//	or a "Critical code" section.
	//
	ASSERT( Critical::critical_code());

	//
	//	Data left to send?
	//
	if( _active->send ) {
		//
		//	Decrement counter and send the next byte.
		//
		_active->send--;
		_dev->write( *( _active->sending++ ));

		//
		//	A byte of data has been sent; there is nothing
		//	more to do until this routine is called again
		//	by the ISR handler.
		//
	}
	else {
		//
		//	Data left to retrieve?
		//
		if( _active->recv ) {
			//
			//	decrement counter and read the last byte
			//	received by the SPI hardware.
			//
			_active->recv--;
			*( _active->receiving++ ) = _dev->read();

			//
			//	If there more to be done?
			//
			if( _active->recv ) {
				//
				//	More data the collect? We need
				//	to push out a packing byte to
				//	drive the shift registers.
				//
				_dev->write( packing_byte );
			}
			else {
				//
				//	No, we have gathered the last byte
				//	of data from the remote device.
				//	The transaction is complete.
				//
				stop_trans();
			}
		}
		else {
			//
			//	Getting here with both send and recv
			//	counts empty means we are done.
			//
			stop_trans();
		}
	}
}


//
//	Define our device handler.
//
SPI_Device spi((SPI_Registers *)0x004C );

//
//	The ISR routine
//
ISR( SPI_STC_vect ) {
	COUNT_INTERRUPT;
	spi.spi_event();
}

//
//	EOF
//


