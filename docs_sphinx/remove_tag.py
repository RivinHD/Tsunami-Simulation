import os
import re

tag_start = '<code class="docutils literal notranslate">'
tag_end = '</code>'

# search in the whole directory path
def search_directory(directory):
    for filename in os.listdir(directory):
        path = os.path.join(directory, filename)
        if not os.path.isfile(path):
            continue
        print(f"Remove Tag from file at {path}")
        with open(path, 'r+', encoding='utf-8') as file:
            lines = file.readlines()
            # remove the given tag
            new_lines = [re.sub(f"{tag_start}(.*?){tag_end}", "\g<1>", line) for line in lines]
        with open(path, 'w', encoding='utf-8') as file:
            file.writelines(new_lines)
                  

if __name__ == "__main__":
    print("Started Remove Tag")
    directory = os.path.join("_build", "html", "api")
    search_directory(directory)
    print("Finished Removing Tag")