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
	mailbox* mb, * next_mb=NULL,*final_mb = NULL;
	mb = (mailbox*)arg;
	int iniprobe = 1,inisort = 1;
	
	/*
	 * FIXME
	 * Replace this comment with your code.
	 *
	 */
	while(1){
		if(!queue_is_empty(&mb->q)){
			message* msg = mailbox_receive(mb);
			if(msg->type == ID){
				pthread_mutex_lock(&id_counter_lock);
				id = msg->payload.integer;
				free(msg);
				pthread_mutex_unlock(&id_counter_lock);
				//printf("%d\n",id);
			}
			if(msg->type == MAILBOX){
				pthread_mutex_lock(&mail_counter_lock);
				//printf("Hello1");
				next_mb = msg->payload.mb;
				free(msg);
				mailboxCounter++;
				if(mailboxCounter>=num_threads){tring_signal();}
				pthread_mutex_unlock(&mail_counter_lock);
			}
			if(msg->type == PING){
				//printf("Hello7");
				pong(id);
				if(next_mb!=NULL)
					mailbox_send(next_mb, msg);
				else
					tring_ping_signal();
			}
			if(msg->type == PRINT){
				//printf("Hello8");
				if(next_mb!=NULL){
					//printf("%d\n",id);
					tring_print(id,nextId);
					mailbox_send(next_mb, msg);
				}else{
					//printf("l%d\n",id);
					tring_printLast(id);
					tring_nid_signal();
				}
			}
			if(msg->type == NID && next_mb!=NULL){
				pthread_mutex_lock(&nid_counter_lock);
				//printf("Hello2");
				if(nidcounter<num_threads){
					//printf("nid\n");
					nextId = msg->payload.integer;
					msg->payload.integer = id;
					mailbox_send(next_mb, msg);
					nidcounter++;
				}else{
					tring_nid_signal();
				}
				pthread_mutex_unlock(&nid_counter_lock);
			}
			if(msg->type == INIPROBE && next_mb!=NULL){
				pthread_mutex_lock(&iniprobe_counter_lock);
				//printf("Hello3");
				if(iniprobeCounter<num_threads && iniprobe){
					if(iniprobeCounter<(num_threads-1))
						mailbox_send(next_mb, msg);
					//printf("iniprobe=%d,%d\n",id,iniprobeCounter);
					message *pmsg;	
					pmsg = NEW_MSG;
					pmsg->type = PROBE;
					pmsg->payload.integer = id;
					mailbox_send(next_mb, pmsg);
					iniprobeCounter++;
					iniprobe = 0;
				}
				if(iniprobeCounter>=(num_threads))
						pthread_mutex_unlock(&probe_counter_lock);
					
				pthread_mutex_unlock(&iniprobe_counter_lock);

			}	
			if(msg->type == PROBE && next_mb!=NULL){
				pthread_mutex_lock(&probe_counter_lock);
				//printf("Hello4");
				int newId = msg->payload.integer;
				if(id>lastID){
					lastID = id;
				}
				if(newId != id){
					if((nextId > id && newId > id && newId < nextId) || (id >= nextId && newId > id)){
					int temp;
					temp = nextId;
					nextId = newId;
					//newId = temp;
					}
					
					msg->payload.integer = newId;
					//printf("%d,%d-%d\n",id,nextId,newId);
					mailbox_send(next_mb, msg);
				}else{
					//printf("%d,%d,%d\n",id,nextId,newId);
					free(msg);
					//printf("Probe Counter = %d\n",probeCounter);
					probeCounter++;
					if(probeCounter>=(num_threads)){tring_probe_signal();}
				}
				pthread_mutex_unlock(&probe_counter_lock);
			}
			if(msg->type == INISORT && next_mb!=NULL){
				pthread_mutex_lock(&ini_sort_lock);
				//printf("Hello5");
				if(iniSortCounter<num_threads && inisort){
					//printf("iniSort=%d\n",iniSortCounter);
					if(iniSortCounter<(num_threads-1))
						mailbox_send(next_mb, msg);
					iniSortCounter++;
					inisort = 0;
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
				}
				if(iniSortCounter>=(num_threads))
						pthread_mutex_unlock(&sort_lock);
				pthread_mutex_unlock(&ini_sort_lock);
			}
			if(msg->type == SORT && next_mb!=NULL){
				pthread_mutex_lock(&sort_lock);
				//printf("Hello6");
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
						//mailbox_send(next_mb, msg);
						//mailbox_send(next_mb, mmsg);
					}
					if(newId==id){	
						//printf("SSort=%d,%d\n",id,newId);
						sortCounter++;
						if(sortCounter>=(num_threads)){
							tring_sort_signal();
						}
						free(msg);
						free(mmsg);
					}else{
						mailbox_send(next_mb, msg);
						mailbox_send(next_mb, mmsg);
					}
				}else{
					printf("Error=%d,%d\n",id,mmsg->type);
				}
				pthread_mutex_unlock(&sort_lock);
			}
			if(msg->type == FINILIZE){
				next_mb = final_mb;
				if(next_mb!=NULL)
					mailbox_send(next_mb,msg);
				else
					tring_finilize_signal();
			
			}
			if(msg->type == SHUTDOWN){
				if(next_mb!=NULL)
					mailbox_send(next_mb, msg);
				free(mb);
				pthread_mutex_lock(&shutdown_counter_lock);
				shutdownCounter++;
				pthread_mutex_unlock(&shutdown_counter_lock);
				if(shutdownCounter>=num_threads){tring_signal();}
				return NULL;
			}	
		}
	}
	return NULL;
}
