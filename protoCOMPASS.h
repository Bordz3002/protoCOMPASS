#ifndef protoCOMPASS_H
#define protoCOMPASS_H
#include <Arduino.h>
#include <protoVECTOR.h>
#include <QMC5883LCompass.h>
template<int arr_size, typename type>
class SMA{
  public:
    static constexpr unsigned int invalid=999;
    type data_arr[arr_size];
    unsigned int bottom_data=data_arr[0];
    unsigned int top_data=data_arr[arr_size-1];
    SMA(){
      for(int i=0; i<arr_size; i++){
        data_arr[i]=invalid;
      }
    }
    type sma(unsigned int data){
      unsigned int equals_counter=0;
      unsigned int selected_index;
      //search for vacant
      for(int i=0; i<arr_size; i++){
        if(data_arr[i]!=invalid && i<arr_size-1){ continue;}
        if(data_arr[i]==invalid && i<arr_size-1){ selected_index=i; break;}
        if(data_arr[i]!=invalid && i==arr_size-1){ //full need resorting
          for(int j=0; j<arr_size; j++){
            if(data_arr[j]!=invalid && data==data_arr[j]){  equals_counter++;}
            else{ continue;}
          }
          if(equals_counter==0){
            for(int j=0; j<arr_size-1; j++){
              data_arr[j]=data_arr[j+1];
            }
            data_arr[arr_size-1]=data;
            //averagin
            int total=0;
            for(int j=0; j<arr_size; j++){
              total+=data_arr[j];
            }
            return static_cast<type>(total/arr_size);
          }
          if(equals_counter!=0){
            int total=0;
            for(int j=0; j<arr_size; j++){
              total+=data_arr[j];
            }
            return static_cast<type>(total/arr_size);
          }
        }
        if(data_arr[i]==invalid && i==arr_size-1){//not full, select slot
          for(int j=0; j<arr_size; j++){
            if(data_arr[j]!=invalid && data==data_arr[j]){  equals_counter++;}
            else{ continue;}
          }
          if(equals_counter==0){
            data_arr[arr_size-1]=data;
            int total=0;
            for(int j=0; j<arr_size; j++){
              total+=data_arr[j];
            }
            return static_cast<type>(total/arr_size);
          }
          if(equals_counter!=0){
            return static_cast<type>(data_arr[arr_size-2]);
          }
        }
      }
      //invalid full check
      for(int i=0; i<arr_size; i++){
        if(data_arr[i]!=invalid && data==data_arr[i]){  equals_counter++;}
        else{ continue;}
      }
      //write
      //if not full, with full invalid check
      if(equals_counter==0 && selected_index!=arr_size-1){ data_arr[selected_index]=data; return static_cast<type>(data_arr[selected_index]);}
    }
};
template <int array_size, typename T=unsigned int>
class proto_compass: public SMA<array_size, T>{
  private:
    QMC5883LCompass compass;
  public:
    proto_compass(): SMA<array_size, T>(){ }
    void begin(){
      compass.init();
    }
    T raw_angle(void){
        compass.read();
        int raw_data=compass.getAzimuth();
        return static_cast<T>((raw_data<0)? raw_data+360 : raw_data);
    }
    T sma_angle(void){
      unsigned int data=raw_angle();
      return static_cast<T>(this->sma(data));
    }
};
#endif