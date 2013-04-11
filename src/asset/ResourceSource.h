/* 
 * the property tree of a source to load the resource
 */
class ResourceSource {
public:
	class Node {
	public:
		virtual Node* firstChild() = 0;
		virtual Node* nextSibling() = 0;
		virtual Node* childByName(char const*) = 0;
		virtual Attribute* firstAttribute() = 0;
		virtual Attribute* attributeByName(char const*) = 0;
	};

	virtual Node* root() = 0;
};
