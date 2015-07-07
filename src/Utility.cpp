//Author: Xuepeng Fan, xuepeng_fan@163.com, 2015.07.07
#include "Utility.h"
#include <fstream>
#include <iostream>


namespace ff
{
bool HighResolutionTimer::s_initialized = false;
LARGE_INTEGER HighResolutionTimer::s_frequency;

milliseconds_t HighResolutionTimer::current_time(){
  if(!s_initialized){
    s_initialized = true;
    QueryPerformanceFrequency(&s_frequency);
  }
  LARGE_INTEGER t;
  QueryPerformanceCounter(&t);
  t.QuadPart *= 1000;
  t.QuadPart /= s_frequency.QuadPart;

  return t.QuadPart;
}


RTProfiler::RTProfiler(const char * fp)
  : mstr_fp(fp){
    std::ofstream ss(mstr_fp);
    ss.close();
  }

  RTProfiler::~RTProfiler(){
    dump_to_file();
  }


void RTProfiler::record(event_id_t event_id, int event_value){
  RecordItem<int> ri;
  ri.time_stamp = HighResolutionTimer::current_time();
  ri.event_id = event_id;
  ri.event_value = event_value;

  m_oIntItems.push_back(ri);

  if(m_oIntItems.size() >= 1024)
    dump_to_file();
}

void RTProfiler::dump_to_file(){
  if(m_oIntItems.size() == 0)
    return ;
  std::ofstream ss(mstr_fp, std::ios::app);
  if(!ss){
    std::cout<<"cannot open file: "<<mstr_fp<<std::endl;
  }
  for(size_t i = 0; i < m_oIntItems.size(); ++i){
    ss<<m_oIntItems[i].time_stamp<<" "<<m_oIntItems[i].event_id<<" "<<m_oIntItems[i].event_value<<"\n";
  }
  ss.close();
  m_oIntItems.clear();
}
}
