import openai
import sys
import os
from dotenv import load_dotenv

load_dotenv()

if __name__ == '__main__':

    # python3 <filename> num_paragraphs query.txt
    if len(sys.argv) < 3:
        print("Usage: python3 api_call.py num_paragraphs query.txt")
        sys.exit(1)

    api_key = os.getenv("OPENAI_API_KEY")
    if not api_key:
        print("Error: OPENAI_API_KEY not set in .env file")
        sys.exit(1)
    openai.api_key = api_key
    num_paragraphs = int(sys.argv[1])
    print(num_paragraphs)

    # Read the paragraphs from the files
    paragraphs = []

    for i in range(num_paragraphs):
        filename = 'paragraph_' + str(i) + '.txt'
        print(filename)
        with open(filename, 'r') as f:
            paragraphs.append(f.read())
            paragraphs.append('\n')
    
    # add query
    query_file = sys.argv[2]
    with open(query_file, 'r') as f:
        query = f.read()
        paragraphs.append(query)
        paragraphs.append('\n')

    # convert paragraphs to a single string
    paragraphs = '\n'.join(paragraphs)

    print(paragraphs)

    query = {
        "role": "user", "content": paragraphs
    }

    chat = openai.ChatCompletion.create(
        model="gpt-3.5-turbo",
        messages=[query]
    )

    reply = chat.choices[0].message.content
    print(reply)