#include <pthread.h>
#include "caltrain.h"



void
station_init(struct station *station)
{
	station->wait_passengers = 0;
	station->on_board = 0;
	pthread_mutex_init(&(station->train_mutex) , NULL);
	pthread_cond_init(&(station->condition_arrival) , NULL);
	pthread_cond_init(&(station->condition_full) , NULL);
}

void
station_load_train(struct station *station, int count)
{
	station->empty_places = count;
	/*send signal for all threads*/
		pthread_cond_broadcast(&(station->condition_arrival));
	/*lock on mutex*/
	pthread_mutex_lock(&(station->train_mutex));	
	while(station->on_board != 0 || ((station->wait_passengers != 0)
	&&(station->empty_places != 0))){
		/*release the lock to make the other threads satisfy the condition*/
		pthread_cond_wait(&(station->condition_full) ,&(station->train_mutex));
	}
	pthread_mutex_unlock(&(station->train_mutex));
	station->empty_places = 0;
}

void
station_wait_for_train(struct station *station)
{
	pthread_mutex_lock(&(station->train_mutex));
	/*increment number of passengers*/
	station->wait_passengers++;
	/*wait for empty places*/
	while(station->empty_places == 0){
		pthread_cond_wait(&(station->condition_arrival) , &(station->train_mutex));
	}
	station->wait_passengers--;
	station->empty_places--;
	station->on_board++;
	pthread_mutex_unlock(&(station->train_mutex));
}

void
station_on_board(struct station *station)
{
	pthread_mutex_lock(&(station->train_mutex));
	station->on_board--;
	if(station->on_board == 0){
		pthread_cond_signal(&(station->condition_full));
	}
	pthread_mutex_unlock(&(station->train_mutex));
}
