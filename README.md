# 1. Introduction 
This project implements a language interpreter in C. It allows users to manage subjects, 
items, and locations within an inventory system while processing the inputs within a proper 
grammar. The system supports actions such as buying, selling, and moving items between 
locations. It also supports condition checking and answers the questions in 
the console input section. 
# 2. Purpose 
This project aims to provide a simple yet robust inventory management 
solution that can be easily integrated into various applications requiring inventory tracking 
and management functionalities. 
# 3. Design Overview 
The system is designed using a combination of data structures including structs, arrays, 
and hashmaps. Key components of the design include: 
• Subject: Represents a human within the inventory system. Contains information 
about the subject name, location, and associated items. 
• Item: Represents an individual item belonging to a subject. Includes information on the item name and count. 
• Location: Represents a physical or logical location within the inventory system, 
such as a warehouse or store. Contains information about the location name and 
associated subjects. 
• Global Arrays: Uses global uninitialized variables to efficiently store and retrieve 
subjects by their names. 
• Locations: Implements a similar global array structure for storing locations. 
# 4. Implementation Details 
• The code does dynamic memory allocation for structs and strings to handle variable 
data sizes. 
• Arrays are used to efficiently store and retrieve subjects and locations based on 
their names. 
• Various functions are implemented to perform actions such as inserting, retrieving, 
and deleting elements from the hash maps. 
• Validation functions are provided to ensure data integrity and handle edge cases. 
• Action functions are implemented to perform buy, sell, move, and other operations 
on subjects and items. 
# 5. Challenges and Solutions 
• Memory Management: Dynamic memory allocation and deallocation were 
managed carefully to avoid memory leaks and segmentation faults. 
• Data Validation: Validating user input and ensuring data integrity posed challenges, 
which were addressed by implementing robust validation functions. 
• Algorithm Efficiency: Designing efficient algorithms for global array operations and 
action functions requires careful consideration of data structures and algorithms.





# For a detailed explanation of the program please read the description.txt

