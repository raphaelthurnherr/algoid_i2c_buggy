/*
 ============================================================================
 Name        : linux_JSON.c
 Author      : Raph
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#define KEY_TO "{'MsgTo'"
#define KEY_FROM "{'MsgFrom'"
#define KEY_MSGID "{'MsgID'"
#define KEY_MESSAGE ""
#define KEY_MESSAGE_TYPE "{'MsgData'{'MsgType'"
#define KEY_MESSAGE_PARAM "{'MsgData'{'MsgParam'"
#define KEY_MESSAGE_VALUE "{'MsgData'{'MsgValue'"

#define KEY_MESSAGE_VALUE_DIN "{'MsgData'{'MsgValue'[*{'din'"

#define KEY_MESSAGE_VALUE_SONAR "{'MsgData'{'MsgValue'[*{'sonar'"
#define KEY_MESSAGE_VALUE_ANGLE "{'MsgData'{'MsgValue'[*{'angle'"
#define KEY_MESSAGE_VALUE_BATT "{'MsgData'{'MsgValue'[*{'battery'"

#define KEY_MESSAGE_VALUE_EVENT_STATE "{'MsgData'{'MsgValue'[*{'event'"
#define KEY_MESSAGE_VALUE_EVENT_LOWER "{'MsgData'{'MsgValue'[*{'event_lower'"
#define KEY_MESSAGE_VALUE_EVENT_HIGHER "{'MsgData'{'MsgValue'[*{'event_higher'"

#define KEY_MESSAGE_VALUE_WHEEL "{'MsgData'{'MsgValue'[*{'wheel'"
#define KEY_MESSAGE_VALUE_VELOCITY "{'MsgData'{'MsgValue'[*{'velocity'"
#define KEY_MESSAGE_VALUE_TIME "{'MsgData'{'MsgValue'[*{'time'"
#define KEY_MESSAGE_VALUE_CM "{'MsgData'{'MsgValue'[*{'cm'"
#define KEY_MESSAGE_VALUE_ACCEL "{'MsgData'{'MsgValue'[*{'accel'"
#define KEY_MESSAGE_VALUE_DECEL "{'MsgData'{'MsgValue'[*{'decel'"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../buggy_descriptor.h"
#include "linux_json.h"
#include "libs/lib_json/jRead.h"
#include "libs/lib_json/jWrite.h"

void ackToJSON(char * buffer, int msgId, char* to, char * from, char * msgType,char * msgParam, unsigned char valStr, unsigned char count);
char GetAlgoidMsg(ALGOID destMessage, char *srcBuffer);

ALGOID myReplyMessage;

// -----------------------------------------------------------------------------
// MAIN, APPLICATION PRINCIPALE-------------------------------------------------
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// GetAlgoidMsg
// Get message from buffer and set in the message structure
// -----------------------------------------------------------------------------

char GetAlgoidMsg(ALGOID destMessage, char *srcBuffer){
	struct jReadElement element;
	int i;
	int result;


	// ENTETE DE MESSAGE
		jRead_string((char *)srcBuffer, KEY_TO, AlgoidMessageRX.msgTo, 15, NULL );
		jRead_string((char *)srcBuffer, KEY_FROM, AlgoidMessageRX.msgFrom, 15, NULL );
		AlgoidMessageRX.msgID= jRead_int((char *)srcBuffer,  KEY_MSGID, NULL);

	// MESSAGE TYPE
				char myDataString[20];
				// Clear string
				for(i=0;i<20;i++) myDataString[i]=0;

				jRead_string((char *)srcBuffer,  KEY_MESSAGE_TYPE,myDataString,15, NULL);

				AlgoidMessageRX.msgType= -1;
				if(!strcmp(myDataString, "command")) AlgoidMessageRX.msgType = COMMAND;
				if(!strcmp(myDataString, "request")) AlgoidMessageRX.msgType = REQUEST;
				if(!strcmp(myDataString, "ack")) AlgoidMessageRX.msgType = ACK;
				if(!strcmp(myDataString, "response")) AlgoidMessageRX.msgType = RESPONSE;
				if(!strcmp(myDataString, "event")) AlgoidMessageRX.msgType = EVENT;
				if(!strcmp(myDataString, "negoc")) AlgoidMessageRX.msgType = NEGOC;
				if(!strcmp(myDataString, "error")) AlgoidMessageRX.msgType = ERROR;

	// MESSAGE PARAM
				// Clear string
				for(i=0;i<20;i++) myDataString[i]=0;
				jRead_string((char *)srcBuffer,  KEY_MESSAGE_PARAM,myDataString,15, NULL);

				AlgoidMessageRX.msgParam=-1;
					if(!strcmp(myDataString, "stop")) AlgoidMessageRX.msgParam = STOP;
					if(!strcmp(myDataString, "move")) AlgoidMessageRX.msgParam = MOVE;
					if(!strcmp(myDataString, "2wd")) AlgoidMessageRX.msgParam = LL_2WD;

					if(!strcmp(myDataString, "distance")) AlgoidMessageRX.msgParam = DISTANCE;
					if(!strcmp(myDataString, "battery")) AlgoidMessageRX.msgParam = BATTERY;
					if(!strcmp(myDataString, "din")) AlgoidMessageRX.msgParam = DINPUT;


				  jRead((char *)srcBuffer, KEY_MESSAGE_VALUE, &element );

					// RECHERCHE DATA ARRAY
				  if(element.dataType == JREAD_ARRAY ){
					  AlgoidMessageRX.msgValueCnt=element.elements;
				      for(i=0; i<element.elements; i++ )    // loop for no. of elements in JSON
				      {
				    	  if(AlgoidMessageRX.msgParam == LL_2WD){
				    		  result = jRead_string((char *)srcBuffer, KEY_MESSAGE_VALUE_WHEEL, myDataString, 15, &i );
				    		  if(!strcmp(myDataString, "left")) AlgoidMessageRX.msgValArray[i].wheel = MOTOR_LEFT;
				    		  if(!strcmp(myDataString, "right")) AlgoidMessageRX.msgValArray[i].wheel = MOTOR_RIGHT;

					    	  AlgoidMessageRX.msgValArray[i].velocity= jRead_long((char *)srcBuffer, KEY_MESSAGE_VALUE_VELOCITY, &i);
					    	  AlgoidMessageRX.msgValArray[i].time= jRead_long((char *)srcBuffer, KEY_MESSAGE_VALUE_TIME, &i);
					    	  AlgoidMessageRX.msgValArray[i].cm= jRead_long((char *)srcBuffer, KEY_MESSAGE_VALUE_CM, &i);
					    	  AlgoidMessageRX.msgValArray[i].accel= jRead_long((char *)srcBuffer, KEY_MESSAGE_VALUE_ACCEL, &i);
					    	  AlgoidMessageRX.msgValArray[i].decel= jRead_long((char *)srcBuffer, KEY_MESSAGE_VALUE_DECEL, &i);
				    	  }

				    	  if(AlgoidMessageRX.msgParam == DINPUT){
						     AlgoidMessageRX.DINsens[i].id= jRead_long((char *)srcBuffer, KEY_MESSAGE_VALUE_DIN, &i);
				    		 jRead_string((char *)srcBuffer, KEY_MESSAGE_VALUE_EVENT_STATE, AlgoidMessageRX.DINsens[i].event_state, 15, &i );
//					    	 printf("id:%d event: %s\n", AlgoidMessageRX.DINsens[i].id, AlgoidMessageRX.DINsens[i].event_state);
				    	  }

				    	  if(AlgoidMessageRX.msgParam == DISTANCE){
				    		  jRead_string((char *)srcBuffer, KEY_MESSAGE_VALUE_EVENT_STATE, AlgoidMessageRX.DISTsens[i].event_state, 15, &i );
				    		  AlgoidMessageRX.DISTsens[i].id= jRead_long((char *)srcBuffer, KEY_MESSAGE_VALUE_SONAR, &i);
				    		  AlgoidMessageRX.DISTsens[i].angle= jRead_long((char *)srcBuffer, KEY_MESSAGE_VALUE_ANGLE, &i);
				    		  AlgoidMessageRX.DISTsens[i].event_low= jRead_long((char *)srcBuffer, KEY_MESSAGE_VALUE_EVENT_LOWER, &i);
				    		  AlgoidMessageRX.DISTsens[i].event_high= jRead_long((char *)srcBuffer, KEY_MESSAGE_VALUE_EVENT_HIGHER, &i);
//				    		  printf("\n-SONAR: %d DIST_EVENT_HIGH: %d, DIST_EVENT_LOW: %d  DIST_EVENT_ENABLE: %s\n", AlgoidMessageRX.DISTsens[i].id,
//				    		  AlgoidMessageRX.DISTsens[i].event_high, AlgoidMessageRX.DISTsens[i].event_low, AlgoidMessageRX.DISTsens[i].event_state);
				    	  }

				    	  if(AlgoidMessageRX.msgParam == BATTERY){
				    		  jRead_string((char *)srcBuffer, KEY_MESSAGE_VALUE_EVENT_STATE, AlgoidMessageRX.BATTsens[i].event_state, 15, &i );
				    		  AlgoidMessageRX.BATTsens[i].id= jRead_long((char *)srcBuffer, KEY_MESSAGE_VALUE_BATT, &i);
				    		  AlgoidMessageRX.BATTsens[i].event_low= jRead_long((char *)srcBuffer, KEY_MESSAGE_VALUE_EVENT_LOWER, &i);
				    		  AlgoidMessageRX.BATTsens[i].event_high= jRead_long((char *)srcBuffer, KEY_MESSAGE_VALUE_EVENT_HIGHER, &i);				    	  }

				    	  if(result == 0)
				    		  return 0;
				    }
				  }else AlgoidMessageRX.msgValueCnt=0;

				  if(AlgoidMessageRX.msgParam < 0 || AlgoidMessageRX.msgType < 0){
					  return 0;
				  }

				  return 1;

}


// -----------------------------------------------------------------------------
// replyToHost
// convert the structure in JSON format & Send to host
// -----------------------------------------------------------------------------
void ackToJSON(char * buffer, int msgId, char* to, char* from, char* msgType, char* msgParam, unsigned char valStr, unsigned char count ){
	unsigned int buflen= 1024;
	unsigned char i;

// Formatage de la r�ponse en JSON
	jwOpen( buffer, buflen, JW_OBJECT, JW_PRETTY );		// start root object
		jwObj_string( "MsgTo", to );				// add object key:value pairs
		jwObj_string( "MsgFrom", from );				// add object key:value pairs
		jwObj_int( "MsgID", msgId );
		jwObj_object( "MsgData" );
			jwObj_string( "MsgType", msgType );				// add object key:value pairs
			if(msgParam!=0) jwObj_string( "MsgParam", msgParam );				// add object key:value pairs

			if(count>0){

				jwObj_array( "MsgValue" );
				for(i=0;i<count;i++){
					//printf("Make array: %d values: %d %d\n", i, 0,9);
					jwArr_object();
						switch(valStr){
						case DISTANCE :	jwObj_int("sonar",AlgoidResponse[i].DISTresponse.id);				// add object key:value pairs
										jwObj_int("cm", round((AlgoidResponse[i].value)/10));				// add object key:value pairs
										jwObj_int("angle", AlgoidResponse[i].DISTresponse.angle);				// add object key:value pairs
										jwObj_string("event", AlgoidResponse[i].DISTresponse.event_state);				// add object key:value pairs
										jwObj_int("event_lower", AlgoidResponse[i].DISTresponse.event_low);				// add object key:value pairs
										jwObj_int("event_higher", AlgoidResponse[i].DISTresponse.event_high);				// add object key:value pairs
										break;

						case BATTERY :
										jwObj_int( "battery",AlgoidResponse[i].BATTesponse.id);				// add object key:value pairs
										jwObj_int("mV", AlgoidResponse[i].value);				// add object key:value pairs
										jwObj_string("event", AlgoidResponse[i].BATTesponse.event_state);				// add object key:value pairs
										jwObj_int("event_lower", AlgoidResponse[i].BATTesponse.event_low);				// add object key:value pairs
										jwObj_int("event_higher", AlgoidResponse[i].BATTesponse.event_high);				// add object key:value pairs
									    break;

						case DINPUT :
										jwObj_int("din",AlgoidResponse[i].DINresponse.id);				// add object key:value pairs
										jwObj_int( "State", AlgoidResponse[i].value);				// add object key:value pairs
										jwObj_string("event", AlgoidResponse[i].DINresponse.event_state);				// add object key:value pairs
									   break;
						default:  	   break;

						}
					jwEnd();
				}
				jwEnd();
			}
		jwEnd();
		jwClose();
}
