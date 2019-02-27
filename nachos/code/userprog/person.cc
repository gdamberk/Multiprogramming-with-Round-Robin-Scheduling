#include "person.h"

person::person(int s, int p, int c, int l){
	seq = s;
	priority = p;
	clothes = c;
	laundryTime = l;
	order = o;
}

void person::setSeq(int s){
	seq = s;
}

void person::setPriority(int p){
	priority = p;
}

void person::setClothes(int c){
	clothes = c;
}

void person::setLaundryTime(int l){
	laundryTime = l;
}

void person::setOrder(int o){
	order = o;
}

int person::getSeq(){
	return seq;
}

int person::getPriority(){
	return priority;
}

int person::getClothes(){
	return clothes;
}

int person::getLaundryTime(){
	return laundryTime;
}

int person::getOrder(){
	return order;
}