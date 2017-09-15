struct Work{
   1: i32 output = 0,
   2: string message,
   3: optional string comment,
}
service helloSvc {
   string getMessage(1: string name),
   Work get_output()
}

