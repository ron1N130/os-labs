#include <fcntl.h>
#include <unistd.h>
#include "tasks_lib.h"

#ifdef VPL
#include "tasks.h"
#endif


int FCFS(struct device_t* device, int requests[], int size){
  int total = 0;
  int cur = dev_get_cylinder(device);
  
  for(int i=0; i<size; ++i){
      int target = requests[i];
      int diff = target - cur;
      if(diff<0) diff = -diff;
      total += diff;
      dev_move_to_cylinder(device, target);
      dev_work(device);
      cur = target;
  }
  return total;
}

int SSF(struct device_t* device, int requests[], int size){
  int total = 0;
  int cur = dev_get_cylinder(device);
  
  int served[size];
  for(int i = 0; i < size; ++i) served[i] = 0;
  
  int completed = 0;
  while(completed < size){
      int best = -1;
      int best_dist = 0;
      
      for(int i=0;i<size;++i){
          if(served[i]) continue;
          int d = requests[i] - cur;
          if(d<0) d = -d;
          
          if(best==-1 || d<best_dist || (d==best_dist && requests[i] < requests[best])){
              best = i;
              best_dist = d;
          }
      }
      
      int target = requests[best];
      total += best_dist;
      dev_move_to_cylinder(device,target);
      dev_work(device);
      cur = target;
      
      served[best] = 1;
      ++completed;
  }
  return total;
}

int SCAN(struct device_t* device, int requests[], int size){
    int total = 0;
    int cur = dev_get_cylinder(device);
    const int start_pos = cur;
    const int maxc = dev_get_max_cylinder(device);
    int dir_val = (int) dev_get_direction(device);
    int going_up = dir_val > 0;
    
    int sorted[size];
    for(int i = 0; i < size; ++i) sorted[i] = requests[i];
    for(int i = 0; i < size; ++i){
        int key = sorted[i];
        int j = i-1;
        while(j >= 0 && sorted[j] > key){
            sorted[j+1] = sorted[j];
            --j;
        }
        sorted[j+1] = key;
    }
    
    if(going_up){
        for(int i = 0; i < size; ++i){
            if(sorted[i] >= cur){
                int diff = sorted[i] - cur;
                total += diff;
                dev_move_to_cylinder(device, sorted[i]);
                dev_work(device);
                cur = sorted[i];
            }
        }
        
        if(cur != maxc){
            int diff = maxc - cur;
            total += diff;
            dev_move_to_cylinder(device, maxc);
            cur = maxc;
        }
        
        dev_switch_direction(device);
        
        for(int i = size-1; i>= 0; --i){
            if(sorted[i] < start_pos){
                int diff = cur - sorted[i];
                total += diff;
                dev_move_to_cylinder(device, sorted[i]);
                dev_work(device);
                cur = sorted[i];
            }
        }
    }else{
        
        for(int i = size-1; i>=0; --i){
            if(sorted[i] <= cur){
                int diff = cur - sorted[i];
                total += diff;
                dev_move_to_cylinder(device, sorted[i]);
                dev_work(device);
                cur = sorted[i];
            }
        }
        
        if(cur != 0){
            total += cur;
            dev_move_to_cylinder(device, 0);
            cur = 0;
        }
        dev_switch_direction(device);
        
        for(int i = 0; i < size; ++i){
            if(sorted[i] > start_pos){
                int diff = sorted[i] - cur;
                total += diff;
                dev_move_to_cylinder(device, sorted[i]);
                dev_work(device);
                cur = sorted[i];
            }
        }
    }
    
    return total;
} 
