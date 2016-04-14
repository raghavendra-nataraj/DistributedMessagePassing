/*
 * This file contains the code for a extra credit assignment worker thread.
 * The main thread of execution will start a thread by using tring_thread_start.
 * Most of a student's work will go into this file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "mailbox.h"
#include "util.h"

/*
 * void* tring_thread_start(void* arg)
 *
 * This function is used in the creating of worker threads for this assignment.
 * Its parameter 'arg' is a pointer to a thread's designated mailbox.  The
 * thread should exit with a NULL pointer.
 *
 */
void* tring_thread_start(void* arg) {
	int id,nextId;
	int iniProbe = 1;
	mailbox* mb, * next_mb=NULL,*final_mb = NULL;
	mb = (mailbox*)arg;
	
	/*
	 * FIXME
	 * Replace this comment with your code.
	 *
	 */
	while(1){
		if(!queue_is_empty(&mb->q)){
			message* msg = mailbox_receive(mb);
			if(msg->type == ID){
				id = msg->payload.integer;
				free(msg);
				//printf("%d\n",id);
			}
			if(msg->type == MAILBOX){
				next_mb = msg->payload.mb;
				free(msg);
				pthread_mutex_lock(&mail_counter_lock);
				probeCounter++;
				pthread_mutex_unlock(&mail_counter_lock);
				if(probeCounter>=num_threads){tring_signal();}
			}
			if(msg->type == PING){
				pong(id);
				if(final_mb!=NULL)
					mailbox_send(final_mb, msg);
				else
					tring_signal();
			}
			if(msg->type == PRINT){
				if(final_mb!=NULL){
					tring_print(id,nextId);
					mailbox_send(final_mb, msg);
				}else{
					tring_print(id,100);
					tring_signal();
				}
			}
			if(msg->type == NID && next_mb!=NULL){
				if(msg->payload.integer!=id){
					nextId = msg->payload.integer;
					msg->payload.integer = id;
					mailbox_send(next_mb, msg);
					pthread_mutex_lock(&nid_counter_lock);
					probeCounter++;
					pthread_mutex_unlock(&nid_counter_lock);
					if(probeCounter>=num_threads){tring_signal();}
				}
			}
			if(msg->type == INIPROBE && next_mb!=NULL){
				if(iniProbe){
					mailbox_send(next_mb, msg);
					message *pmsg;	
					pmsg = NEW_MSG;
					pmsg->type = PROBE;
					pmsg->payload.integer = id;
					mailbox_send(next_mb, pmsg);
					iniProbe = 0;
					//if(probeCounter>=num_threads){tring_signal();}
				}
			}	
			if(msg->type == PROBE && next_mb!=NULL){
				int newId = msg->payload.integer;
				if(id>lastID){
					lastID = id;
				}
				if(newId != id){
					if((nextId > id && newId > id && newId < nextId) || (id > nextId && newId > id)){
					int temp;
					temp = nextId;
					nextId = newId;
					newId = temp;
					}
					
					msg->payload.integer = newId;
					//printf("%d,%d\n",id,nextId);
					mailbox_send(next_mb, msg);
				}else{
					pthread_mutex_lock(&probe_counter_lock);
					free(msg);
					probeCounter = probeCounter+1;
					pthread_mutex_unlock(&probe_counter_lock);
					if(probeCounter>=num_threads){tring_signal();}
				}
			}
			if(msg->type == INISORT && next_mb!=NULL){
				pthread_mutex_lock(&ini_sort_lock);
				if(probeCounter < num_threads-1){
					probeCounter+=1;
					//printf("sent probe %d\n",id);
					mailbox_send(next_mb, msg);
				}
				message *somsg;	
				somsg = NEW_MSG;
				somsg->type = SORT;
				somsg->payload.integer = id;
				mailbox_send(next_mb, somsg);
				message *smsg;	
				smsg = NEW_MSG;
				smsg->type = MAILBOX;
				smsg->payload.mb = mb;
				mailbox_send(next_mb, smsg);
				pthread_mutex_unlock(&ini_sort_lock);
			}
			if(msg->type == SORT && next_mb!=NULL){
				pthread_mutex_lock(&probe_counter_lock);
				int newId = msg->payload.integer;
				if(id == lastID){
					final_mb = NULL;
				}
				if(firstID>=id){
					firstID = id;
					first_mb = mb;
				}
				message* mmsg = mailbox_receive(mb);
				if(mmsg->type == MAILBOX){
					mailbox *new_mb = mmsg->payload.mb;
					if(newId==nextId && id!=lastID){
						//printf("Match =%d- %d,%d\n",id,nextId,newId);
						final_mb = new_mb;
						free(msg);
						free(mmsg);
						sortCounter++;
						if(sortCounter>=(num_threads-1)){
							//printf("signal =%d\n",sortCounter);
							tring_signal();
						}/*else{
							printf("Counter =%d\n",sortCounter);
						}*/
					}else{
						if(newId!=id){
							
							mailbox_send(next_mb, msg);
							mailbox_send(next_mb, mmsg);
						}else{
							free(msg);
							free(mmsg);
						}
					}
				}else{
					printf("Error=%d,%d\n",id,mmsg->type);
				}
				pthread_mutex_unlock(&probe_counter_lock);
			}
			if(msg->type == SHUTDOWN){
				if(final_mb!=NULL)
					mailbox_send(final_mb, msg);
				free(mb);
				pthread_mutex_lock(&shutdown_counter_lock);
				probeCounter++;
				pthread_mutex_unlock(&shutdown_counter_lock);
				if(probeCounter>=num_threads){tring_signal();}
				return NULL;
			}	
		}
	}
	return NULL;
}
