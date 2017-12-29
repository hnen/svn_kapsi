sxstrace trace -logfile:sxstrace.etl -outfile:sxstrace.txt
sxstrace parse -logfile:sxstrace.etl -outfile:sxstrace.txt
@cat sxstrace.txt
