\all: parent_child_q1 parent_child_q2 parent_child_q2_withoutBonus

parent_child_q1: Q1.c
	
	@gcc -o parent_child_q1 Q1.c

parent_child_q2: Q1_part2.c
	
	@gcc -o parent_child_q2 Q1_part2.c

parent_child_q2_withoutBonus : Q1_part2_without_Bonus.c
	
	@gcc -o parent_child_q2_withoutBonus Q1_part2_without_Bonus.c

clean:
	
	@rm -f parent_child_q1 parent_child_q2 parent_child_q2_withoutBonus

run_q1: parent_child_q1
	@./parent_child_q1
	@rm -f parent_child_q1

run_q2: parent_child_q2
	@./parent_child_q2
	@rm -f parent_child_q2


