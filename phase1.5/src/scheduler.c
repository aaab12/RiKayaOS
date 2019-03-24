void scheduler(){
  current_process = NULL;
  if(!(emptyProcQ(&ready_queue))){
    current_process = removeProcQ(&ready_queue);

  }
}
